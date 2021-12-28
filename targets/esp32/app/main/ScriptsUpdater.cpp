#include "ScriptsUpdater.h"
#include "EventBus.h"
#include "WiFiDriver.h"
#include "esp_system.h"
#include <cstring>

#define SCRIPTS_TAR_TEMPLATE                                                   \
    "https://github.com/feelfreelinux/euphonium/releases/download/"            \
    "%s/scripts.tar"

ScriptsUpdater::ScriptsUpdater() {}

bool ScriptsUpdater::versionMatches() {
    std::ifstream indexFile("/spiffs/scripts-version");
    std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
                             std::istreambuf_iterator<char>());
    indexFile.close();

    BELL_LOG(info, "scripts_updater", "Scripts version: %s",
             indexContent.c_str());
    std::string currentVer(EUPH_VERSION);
    BELL_LOG(info, "scripts_updater", "App version: %s", currentVer.c_str());
    if (currentVer == "debug") return true;
    return indexContent.substr(0, currentVer.size()) ==
           std::string(EUPH_VERSION);
}

void ScriptsUpdater::update() {
    mainEventBus = std::make_shared<EventBus>();
    initializeWiFiStack();

    // Prepare lua event thread
    auto subscriber = dynamic_cast<EventSubscriber *>(this);
    mainEventBus->addListener(EventType::LUA_MAIN_EVENT, *subscriber);

    std::ifstream indexFile("/spiffs/wifi.config.json");
    std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
                             std::istreambuf_iterator<char>());
    indexFile.close();

    cJSON *json = cJSON_Parse(indexContent.c_str());
    if (json == NULL) {
        return;
    }

    cJSON *ssid = cJSON_GetObjectItemCaseSensitive(json, "ssid");
    cJSON *password = cJSON_GetObjectItemCaseSensitive(json, "password");
    auto ssidStr = std::string(ssid->valuestring);
    auto passwordStr = std::string(password->valuestring);
    cJSON_Delete(json);

    tryToConnect(ssidStr, passwordStr, false);

    while (true) {
        BELL_SLEEP_MS(100);
        mainEventBus->update();
    }
}

void ScriptsUpdater::downloadUpdate() {
    httpStream = std::make_shared<bell::HTTPStream>();
    auto size = std::snprintf(nullptr, 0, SCRIPTS_TAR_TEMPLATE, EUPH_VERSION);
    std::string output(size + 1, '\0');
    std::sprintf(&output[0], SCRIPTS_TAR_TEMPLATE, EUPH_VERSION);
    BELL_LOG(info, "scripts_updater", "Requested url: %s %d", output.c_str());

    httpStream->connectToUrl(output.substr(0, output.size() - 1));
    BELL_LOG(info, "tar_reader", "Size seems to be %d",
             httpStream->contentLength);
    tarData = std::vector<uint8_t>(httpStream->contentLength);

    int readData = 0;
    while (readData < httpStream->contentLength) {
        int nbytes = httpStream->read(tarData.data() + readData, 128);
        if (nbytes < 0)
            throw std::runtime_error("Failed to read scripts.tar");
        readData += nbytes;
    }

    BELL_LOG(info, "scripts_updater", "Data received %d", tarData.size());
    readTarHeaders();
}

size_t ScriptsUpdater::readFileSize() {
    char *current_char = (char *)&tarData[pos + 124];
    size_t output = 0;
    int size = 11;
    while (size > 0) {
        output = output * 8 + *current_char - '0';
        current_char++;
        size--;
    }
    return output;
}

void ScriptsUpdater::readTarHeaders() {
    if ((pos + 512) >= tarData.size()) {
        esp_restart();
        return;
    }
    auto type = (char)tarData[pos + 156];
    std::string fileName =
        +"/spiffs/" + std::string(&tarData[pos], &tarData[pos + 100]);
    BELL_LOG(info, "scripts_updater", "name: %s", fileName.c_str());

    if (type == '0') {
        int fileSize = readFileSize();
        if (pos + 512 + fileSize > tarData.size()) {
            esp_restart();
            return;
        }

        int remaining = 512 - (fileSize % 512);
        BELL_LOG(info, "scripts_updater", "fileSize: %d", fileSize);

        pos += 512;
        std::ofstream currentFile;
        currentFile.open(fileName, std::ios::binary);
        currentFile.write((char *)&tarData[pos], fileSize);
        currentFile.close();
        pos += fileSize;
        pos += remaining;

        return readTarHeaders();
    } else if (type == '5') {
        BELL_LOG(info, "http", "Got folder, create it");
        mkdir(fileName.c_str(), 0775);
        pos += 512;
        return readTarHeaders();
    } else {
        BELL_LOG(info, "http", "Got not folder, skip!");
        pos += 512;
        return readTarHeaders();
    }
}

void ScriptsUpdater::handleEvent(std::unique_ptr<Event> event) {
    BELL_LOG(info, "scripts_updater", "handleEvent");
    if (event->subType == "wifiStateChanged") {
        auto eventData = event->toBerry();
        if (std::any_cast<std::string>(eventData["state"]) == "connected") {
            BELL_LOG(info, "scripts_updater", "WiFi connected");
            downloadUpdate();
        }
    }
}
