#include "CaptivePortalTask.h"
#include <mutex>

using namespace euph;

CaptivePortalTask::CaptivePortalTask()
    : bell::Task("CaptivePortal", 1024 * 4, 0, 0) {}

void CaptivePortalTask::setn16(void* pp, int16_t n) {
  char* p = (char*)pp;
  *p++ = (n >> 8);
  *p++ = (n & 0xff);
}

CaptivePortalTask::~CaptivePortalTask() {}

void CaptivePortalTask::setn32(void* pp, int32_t n) {
  char* p = (char*)pp;
  *p++ = (n >> 24) & 0xff;
  *p++ = (n >> 16) & 0xff;
  *p++ = (n >> 8) & 0xff;
  *p++ = (n & 0xff);
}

uint16_t CaptivePortalTask::ntohs(uint16_t* in) {
  char* p = (char*)in;
  return ((p[0] << 8) & 0xff00) | (p[1] & 0xff);
}

char* CaptivePortalTask::labelToStr(char* packet, char* labelPtr, int packetSz,
                                    char* res, int resMaxLen) {
  int i, j, k;
  char* endPtr = NULL;
  i = 0;
  do {
    if ((*labelPtr & 0xC0) == 0) {
      j = *labelPtr++;  // skip past length
      // Add separator period if there already is data in res
      if (i < resMaxLen && i != 0)
        res[i++] = '.';
      // Copy label to res
      for (k = 0; k < j; k++) {
        if ((labelPtr - packet) > packetSz)
          return NULL;
        if (i < resMaxLen)
          res[i++] = *labelPtr++;
      }
    } else if ((*labelPtr & 0xC0) == 0xC0) {
      // Compressed label pointer
      endPtr = labelPtr + 2;
      int offset = this->ntohs(((uint16_t*)labelPtr)) & 0x3FFF;
      // Check if offset points to somewhere outside of the packet
      if (offset > packetSz)
        return NULL;
      labelPtr = &packet[offset];
    }
    // check for out-of-bound-ness
    if ((labelPtr - packet) > packetSz)
      return NULL;
  } while (*labelPtr != 0);
  res[i] = 0;  // zero-terminate
  if (endPtr == NULL)
    endPtr = labelPtr + 1;
  return endPtr;
}

char* CaptivePortalTask::strToLabel(char* str, char* label, int maxLen) {
  char* len = label;    // ptr to len byte
  char* p = label + 1;  // ptr to next label byte to be written
  while (1) {
    if (*str == '.' || *str == 0) {
      *len = ((p - len) - 1);  // write len of label bit
      len = p;                 // pos of len for next part
      p++;                     // data ptr is one past len
      if (*str == 0)
        break;  // done
      str++;
    } else {
      *p++ = *str++;  // copy byte
          //if ((p-label)>maxLen) return NULL;	// check out of bounds
    }
  }
  *len = 0;
  return p;  // ptr to first free byte in resp
}

void CaptivePortalTask::dnsRecv(struct sockaddr_in* premoteAddr, char* pusrdata,
                                unsigned short length) {
  char buff[WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_LEN];
  char reply[WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_LEN];
  int i;
  char* rend = &reply[length];
  char* p = pusrdata;
  DNSHeader* hdr = (DNSHeader*)p;
  DNSHeader* rhdr = (DNSHeader*)&reply[0];
  p += sizeof(DNSHeader);

  // Some sanity checks:
  if (length > WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_LEN)
    return;  // Packet is longer than DNS implementation allows
  if (length < sizeof(DNSHeader))
    return;  // Packet is too short
  if (hdr->ancount || hdr->nscount || hdr->arcount)
    return;  // this is a reply, don't know what to do with it
  if (hdr->flags & WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_FLAG_TC)
    return;  // truncated, can't use this
  // Reply is basically the request plus the needed data
  memcpy(reply, pusrdata, length);
  rhdr->flags |= WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_FLAG_QR;

  for (i = 0; i < this->ntohs(&hdr->qdcount); i++) {
    // Grab the labels in the q string
    p = this->labelToStr(pusrdata, p, length, buff, sizeof(buff));
    if (p == NULL)
      return;
    DNSQuestionFooter* qf = (DNSQuestionFooter*)p;
    p += sizeof(DNSQuestionFooter);

    EUPH_LOG(info, TASK,
             "WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_Q (type 0x%X cl 0x%X) for %s",
             this->ntohs(&qf->type), this->ntohs(&qf->cl), buff);

    if (this->ntohs(&qf->type) == WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_A) {
      // They want to know the IPv4 address of something.
      // Build the response.

      rend = this->strToLabel(buff, rend,
                              sizeof(reply) - (rend - reply));  // Add the label
      if (rend == NULL)
        return;
      DNSResourceFooter* rf = (DNSResourceFooter*)rend;
      rend += sizeof(DNSResourceFooter);
      setn16(&rf->type, WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_A);
      setn16(&rf->cl, WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QCLASS_IN);
      setn32(&rf->ttl, 0);
      setn16(&rf->rdlength, 4);  // IPv4 addr is 4 bytes;
      // Grab the current IP of the softap interface

      esp_netif_ip_info_t info;
      esp_netif_get_ip_info(esp_netif_next(NULL), &info);
      *rend++ = ip4_addr1(&info.ip);
      *rend++ = ip4_addr2(&info.ip);
      *rend++ = ip4_addr3(&info.ip);
      *rend++ = ip4_addr4(&info.ip);
      this->setn16(&rhdr->ancount, this->ntohs(&rhdr->ancount) + 1);
    } else if (this->ntohs(&qf->type) ==
               WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_NS) {
      // Give ns server. Basically can be whatever we want because it'll get resolved to our IP later anyway.
      rend = this->strToLabel(buff, rend,
                              sizeof(reply) - (rend - reply));  // Add the label
      DNSResourceFooter* rf = (DNSResourceFooter*)rend;
      rend += sizeof(DNSResourceFooter);
      setn16(&rf->type, WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_NS);
      setn16(&rf->cl, WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QCLASS_IN);
      setn16(&rf->ttl, 0);
      setn16(&rf->rdlength, 4);
      *rend++ = 2;
      *rend++ = 'n';
      *rend++ = 's';
      *rend++ = 0;
      this->setn16(&rhdr->ancount, this->ntohs(&rhdr->ancount) + 1);
    } else if (this->ntohs(&qf->type) ==
               WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_URI) {
      // Give uri to us
      rend = this->strToLabel(buff, rend,
                              sizeof(reply) - (rend - reply));  // Add the label
      DNSResourceFooter* rf = (DNSResourceFooter*)rend;
      rend += sizeof(DNSResourceFooter);
      DNSUriHeader* uh = (DNSUriHeader*)rend;
      rend += sizeof(DNSUriHeader);
      setn16(&rf->type, WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_URI);
      setn16(&rf->cl, WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QCLASS_URI);
      setn16(&rf->ttl, 0);
      setn16(&rf->rdlength, 4 + 16);
      setn16(&uh->prio, 10);
      setn16(&uh->weight, 1);
      memcpy(rend, "http://esp.nonet", 16);
      rend += 16;
      this->setn16(&rhdr->ancount, this->ntohs(&rhdr->ancount) + 1);
    }
  }
  // Send the response
  sendto(this->sockFd, (uint8_t*)reply, rend - reply, 0,
         (struct sockaddr*)premoteAddr, sizeof(struct sockaddr_in));
}

void CaptivePortalTask::stopTask() {
  isRunning = false;
  std::scoped_lock lock(this->runningMutex);
}

void CaptivePortalTask::runTask() {
  std::scoped_lock lock(this->runningMutex);
  isRunning = true;
  struct sockaddr_in server_addr;
  uint32_t ret;
  struct sockaddr_in from;
  socklen_t fromlen;
  char udp_msg[WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_LEN];

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  uint16_t port = 53;
  server_addr.sin_port = htons(&port);
  server_addr.sin_len = sizeof(server_addr);

  do {
    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd == -1) {
      EUPH_LOG(info, TASK, "dns_task failed to create sock!");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  } while (sockFd == -1);

  do {
    ret = bind(sockFd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret != 0) {
      EUPH_LOG(info, TASK, "dns_task failed to bind sock!");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  } while (ret != 0);
  struct timeval timeout;

  // Set timeout to 1 second, so we can exit the loop at some point
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  setsockopt(sockFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

  while (isRunning) {
    memset(&from, 0, sizeof(from));
    fromlen = sizeof(struct sockaddr_in);
    ret =
        recvfrom(sockFd, (uint8_t*)udp_msg, WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_LEN,
                 0, (struct sockaddr*)&from, (socklen_t*)&fromlen);
    if (ret > 0)
      this->dnsRecv(&from, udp_msg, ret);
  }

  isRunning = false;
  ::close(sockFd);
}
