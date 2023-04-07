#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <atomic>
#include <string>
#include "BellTask.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "string.h"

#include "EuphLogger.h"

#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#define DNS_PORT (53)
#define DNS_MAX_LEN (256)

#define OPCODE_MASK (0x7800)
#define QR_FLAG (1 << 7)
#define QD_TYPE_A (0x0001)
#define ANS_TTL_SEC (300)
namespace euph {
class CaptivePortalTask : public bell::Task {
 public:
  CaptivePortalTask();
  ~CaptivePortalTask();

  void runTask() override;
  void stopTask();

 private:
  std::atomic<bool> isRunning = false;
  std::mutex runningMutex;

  /*
    Parse the name from the packet from the DNS name format to a regular .-seperated name
    returns the pointer to the next part of the packet
  */
  char* parseDnsName(char* rawName, char* parsedName, size_t parsedNameMaxLen);

  // Parses the DNS request and prepares a DNS response with the IP of the softAP
  int parseDnsRequest(char* req, size_t reqLen, char* dnsReply,
                      size_t dnsReplyMaxLen);
};
}  // namespace euph
