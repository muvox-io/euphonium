#ifndef EUPH_SCRIPTSUPDATER
#define EUPH_SCRIPTSUPDATER

#include <iostream>
#include <fstream>
#include <EventBus.h>
#include <memory>
#include "WiFiDriver.h"
#include "Core.h"
#include <string>
#include <vector>
#include "HTTPStream.h"
#include <JSONObject.h>
#include <sys/stat.h>
#include <fstream>

class ScriptsUpdater: public EventSubscriber {
private:
    std::vector<uint8_t> tarData;
    size_t pos = 0;
public:
    ScriptsUpdater();
    std::shared_ptr<bell::HTTPStream> httpStream;

    void downloadUpdate();
    bool versionMatches();
    size_t readFileSize();
    void readTarHeaders();
    void update();
    void handleEvent(std::unique_ptr<Event> event);
};

#endif
