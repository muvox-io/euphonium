#pragma once

#include <sys/time.h>
#include <arpa/inet.h>
#include <atomic>
#include <string>
#include "BellTask.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "string.h"
#include <sys/socket.h>

#include "EuphLogger.h"

#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_LEN 512

#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_FLAG_QR (1 << 7)
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_FLAG_AA (1 << 2)
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_FLAG_TC (1 << 1)
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_FLAG_RD (1 << 0)

#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_A 1
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_NS 2
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_CNAME 5
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_SOA 6
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_WKS 11
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_PTR 12
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_HINFO 13
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_MINFO 14
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_MX 15
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_TXT 16
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QTYPE_URI 256

#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QCLASS_IN 1
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QCLASS_ANY 255
#define WIFI_CAPTIVE_PORTAL_ESP_IDF_DNS_QCLASS_URI 256

namespace euph {
class CaptivePortalTask : public bell::Task {
 public:
  CaptivePortalTask();
  ~CaptivePortalTask();

  // DNS header attr
  struct DNSHeader {
    uint16_t id;
    uint8_t flags;
    uint8_t rcode;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
  } __attribute__((packed));

  struct DNSLabel {
    uint8_t len;
    uint8_t data;
  } __attribute__((packed));

  struct DNSQuestionFooter {
    uint16_t type;
    uint16_t cl;
  } __attribute__((packed));

  struct DNSResourceFooter {
    uint16_t type;
    uint16_t cl;
    uint32_t ttl;
    uint16_t rdlength;
  } __attribute__((packed));

  struct DNSUriHeader {
    uint16_t prio;
    uint16_t weight;
  } __attribute__((packed));

  void runTask() override;
  void stopTask();

 private:
  int sockFd;
  std::atomic<bool> isRunning = false;
  std::mutex runningMutex;

  void dnsRecv(struct sockaddr_in* premoteAddr, char* pusrdata,
               unsigned short length);
  char* strToLabel(char* str, char* label, int maxLen);
  char* labelToStr(char* packet, char* labelPtr, int packetSz, char* res,
                   int resMaxLen);
  void setn16(void* pp, int16_t n);
  void setn32(void* pp, int32_t n);
  uint16_t ntohs(uint16_t* in);
};
}  // namespace euph
