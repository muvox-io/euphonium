#ifndef EUPH_SSDP_LISTENER
#define EUPH_SSDP_LISTENER

#include <atomic>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "BellLogger.h"
#include <sstream>
#include <vector>
#include <string>
#include "Task.h"
#include <BellUtils.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

class SSDPListener : bell::Task {
private:
    std::string uuid;
    std::atomic<bool> isRunning = true;
public:
    SSDPListener(std::string& uuid);
    void listenForMulticast();
    void runTask();
};

#endif
