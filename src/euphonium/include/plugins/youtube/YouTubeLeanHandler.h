
#ifndef EUPH_LEAN_HANDLER
#define EUPH_LEAN_HANDLER

#include <atomic>
#include "BellLogger.h"
#include <sstream>
#include <vector>
#include <string>
#include "BerryBind.h"
#include "HTTPClient.h"
#include "Task.h"
#include <BellUtils.h>
#include <string>
#include <cJSON.h>
#include <stdio.h>
#include <time.h>
#include <EventBus.h>
#include "YouTubeEvents.h"

class YouTubeLeanHandler : bell::Task {
private:
    std::string uuid;
    std::atomic<bool> isRunning = true;
    std::shared_ptr<bell::HTTPClient> client;
    std::string gconnectId;
    std::string SID;
    std::string screenId;
    std::shared_ptr<EventBus> eventBus;
    std::string bindUrl;

    std::string baseBindUrl;
    std::string bind2Url;
    size_t ofs = 0;

public:
    YouTubeLeanHandler(std::string& uuid, std::shared_ptr<EventBus> eventBus);
    void runTask();
    void registerBindings(std::shared_ptr<berry::VmState>);
    void registerPairCode(const std::string& pairCode);
    void sendBindData(std::string);
};

#endif
