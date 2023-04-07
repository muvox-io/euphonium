#include "CaptivePortalTask.h"
#include <mutex>
#include "BellUtils.h"

using namespace euph;

// DNS Header Packet
typedef struct __attribute__((__packed__)) {
  uint16_t id;
  uint16_t flags;
  uint16_t qd_count;
  uint16_t an_count;
  uint16_t ns_count;
  uint16_t ar_count;
} dns_header_t;

// DNS Question Packet
typedef struct {
  uint16_t type;
  uint16_t cl;
} dns_question_t;

// DNS Answer Packet
typedef struct __attribute__((__packed__)) {
  uint16_t ptr_offset;
  uint16_t type;
  uint16_t cl;
  uint32_t ttl;
  uint16_t addr_len;
  uint32_t ip_addr;
} dns_answer_t;

CaptivePortalTask::CaptivePortalTask()
    : bell::Task("CaptivePortal", 1024 * 8, 0, 0) {}

CaptivePortalTask::~CaptivePortalTask() {
  stopTask();
}

char* CaptivePortalTask::parseDnsName(char* rawName, char* parsedName,
                                      size_t parsedNameMaxLen) {

  char* label = rawName;
  char* name_itr = parsedName;
  int name_len = 0;

  do {
    int sub_name_len = *label;
    // (len + 1) since we are adding  a '.'
    name_len += (sub_name_len + 1);
    if (name_len > parsedNameMaxLen) {
      return NULL;
    }

    // Copy the sub name that follows the the label
    memcpy(name_itr, label + 1, sub_name_len);
    name_itr[sub_name_len] = '.';
    name_itr += (sub_name_len + 1);
    label += sub_name_len + 1;
  } while (*label != 0);

  // Terminate the final string, replacing the last '.'
  parsedName[name_len - 1] = '\0';
  // Return pointer to first char after the name
  return label + 1;
}

int CaptivePortalTask::parseDnsRequest(char* req, size_t reqLen, char* dnsReply,
                                       size_t dnsReplyMaxLen) {
  if (reqLen > dnsReplyMaxLen) {
    return -1;
  }

  // Prepare the reply
  memset(dnsReply, 0, dnsReplyMaxLen);
  memcpy(dnsReply, req, reqLen);

  // Endianess of NW packet different from chip
  dns_header_t* header = (dns_header_t*)dnsReply;
  EUPH_LOG(info, TASK,
           "DNS query with header id: 0x%X, flags: 0x%X, qd_count: %d",
           ntohs(header->id), ntohs(header->flags), ntohs(header->qd_count));

  // Not a standard query
  if ((header->flags & OPCODE_MASK) != 0) {
    return 0;
  }

  // Set question response flag
  header->flags |= QR_FLAG;

  uint16_t qd_count = ntohs(header->qd_count);
  header->an_count = htons(qd_count);

  int reply_len = qd_count * sizeof(dns_answer_t) + reqLen;
  if (reply_len > dnsReplyMaxLen) {
    return -1;
  }

  // Pointer to current answer and question
  char* cur_ans_ptr = dnsReply + reqLen;
  char* cur_qd_ptr = dnsReply + sizeof(dns_header_t);
  char name[128];

  // Respond to all questions with the ESP32's IP address
  for (int i = 0; i < qd_count; i++) {
    char* name_end_ptr = parseDnsName(cur_qd_ptr, name, sizeof(name));
    if (name_end_ptr == NULL) {
      EUPH_LOG(error, TASK, "Failed to parse DNS question: %s", cur_qd_ptr);
      return -1;
    }

    dns_question_t* question = (dns_question_t*)(name_end_ptr);
    uint16_t qd_type = ntohs(question->type);
    uint16_t qd_class = ntohs(question->cl);

    EUPH_LOG(debug, TASK, "Received type: %d | Class: %d | Question for: %s",
             qd_type, qd_class, name);

    if (qd_type == QD_TYPE_A) {
      dns_answer_t* answer = (dns_answer_t*)cur_ans_ptr;

      answer->ptr_offset = htons(0xC000 | (cur_qd_ptr - dnsReply));
      answer->type = htons(qd_type);
      answer->cl = htons(qd_class);
      answer->ttl = htonl(ANS_TTL_SEC);

      esp_netif_ip_info_t ip_info;
      esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"),
                            &ip_info);
      EUPH_LOG(debug, TASK,
               "Answer with PTR offset: 0x%" PRIX16 " and IP 0x%" PRIX32,
               ntohs(answer->ptr_offset), ip_info.ip.addr);

      answer->addr_len = htons(sizeof(ip_info.ip.addr));
      answer->ip_addr = ip_info.ip.addr;
    }
  }
  return reply_len;
}

void CaptivePortalTask::stopTask() {
  EUPH_LOG(debug, TASK, "Stopping Captive Portal Task");
  isRunning = false;
  std::scoped_lock lock(this->runningMutex);
  EUPH_LOG(debug, TASK, "Stopped Captive Portal Task");
}

void CaptivePortalTask::runTask() {
  std::scoped_lock lock(this->runningMutex);
  isRunning = true;
  char rx_buffer[128];
  char addr_str[128];
  int addr_family;
  int ip_protocol;

  struct sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(DNS_PORT);
  addr_family = AF_INET;
  ip_protocol = IPPROTO_IP;
  inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

  int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
  if (sock < 0) {
    EUPH_LOG(error, TASK, "Unable to create socket: errno %d", errno);
    isRunning = false;
    return;
  }

  int err = 1;
  do {
    err = bind(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
      EUPH_LOG(error, TASK, "Socket unable to bind: errno %d, retrying", errno);
    }
    BELL_SLEEP_MS(1000);
  } while (err != 0);
  EUPH_LOG(error, TASK, "Socket bound, port %d", DNS_PORT);
  struct timeval timeout;

  // Set timeout to 1 second, so we can exit the loop at some point
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

  while (isRunning) {
    struct sockaddr_in6 source_addr;  // Large enough for both IPv4 or IPv6
    socklen_t socklen = sizeof(source_addr);
    int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0,
                       (struct sockaddr*)&source_addr, &socklen);

    // Error occurred during receiving
    if (len < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        // Timeout, just continue the loop
        continue;
      }
      EUPH_LOG(error, TASK, "recvfrom failed: errno %d", errno);
      close(sock);
      break;
    }
    // Data received
    else {
      // Get the sender's ip address as string
      if (source_addr.sin6_family == PF_INET) {
        inet_ntoa_r(((struct sockaddr_in*)&source_addr)->sin_addr.s_addr,
                    addr_str, sizeof(addr_str) - 1);
      } else if (source_addr.sin6_family == PF_INET6) {
        inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
      }

      // Null-terminate whatever we received and treat like a string...
      rx_buffer[len] = 0;

      char reply[DNS_MAX_LEN];
      int reply_len = parseDnsRequest(rx_buffer, len, reply, DNS_MAX_LEN);

      EUPH_LOG(debug, TASK,
               "Received %d bytes from %s | DNS reply with len: %d", len,
               addr_str, reply_len);
      if (reply_len <= 0) {
        EUPH_LOG(error, TASK, "Failed to prepare a DNS reply");
      } else {
        int err = sendto(sock, reply, reply_len, 0,
                         (struct sockaddr*)&source_addr, sizeof(source_addr));
        if (err < 0) {
          EUPH_LOG(error, TASK, "Error occurred during sending: errno %d",
                   errno);
          break;
        }
      }
    }
  }

  if (sock != -1) {
    EUPH_LOG(error, TASK, "Shutting down socket");
    shutdown(sock, 0);
    close(sock);
  }
  isRunning = false;
}
