
#include "YouTubeLeanHandler.h"
#include "HTTPClient.h"
#include "cJSON.h"

YouTubeLeanHandler::YouTubeLeanHandler(std::string &uuid,
                                       std::shared_ptr<EventBus> eventBus)
    : bell::Task("lean", 1024, 0, 1) {
    this->eventBus = eventBus;
    this->uuid = uuid;
    startTask();
}

void YouTubeLeanHandler::runTask() {
    client = std::make_shared<bell::HTTPClient>();
    client->executeGET(
        "https://www.youtube.com/api/lounge/pairing/generate_screen_id");
    screenId = client->readToString();
    auto requestBody = "&screen_ids=" + screenId;

    client->executePOST(
        "https://www.youtube.com/api/lounge/pairing/get_lounge_token_batch",
        requestBody, "application/x-www-form-urlencoded");
    auto body = client->readToString();
    auto root = cJSON_Parse(body.c_str());
    auto screens = cJSON_GetObjectItemCaseSensitive(root, "screens");
    auto screen = cJSON_GetArrayItem(screens, 0);
    auto token = std::string(cJSON_GetStringValue(
        cJSON_GetObjectItemCaseSensitive(screen, "loungeToken")));
    cJSON_Delete(root);
    baseBindUrl =
        "https://www.youtube.com/api/lounge/bc/bind?device=LOUNGE_SCREEN&id=" +
        uuid +
        "&name=Euphonium&app=lb-v4&theme=cl&capabilities=&"
        "mdxVersion=2&loungeIdToken=" +
        token + "&VER=8&v=2&zx=xxxxxxxxxxxx&t=1";

    bindUrl = baseBindUrl + "&RID=1337&AID=42";
    client->executePOST(bindUrl, "&count=0",
                        "application/x-www-form-urlencoded");

    body = client->readToString();

    auto start = body.find("c\",\"") + 4;
    auto end = body.substr(start).find("\"");
    SID = body.substr(start, end);

    start = body.find("S\",\"") + 4;
    end = body.substr(start).find("\"");
    gconnectId = body.substr(start, end);

    bind2Url = baseBindUrl + "&SID=" + SID + "&gsessionid=" + gconnectId +
               "&RID=rpc&type=xmlhttp&CI=0&AID=3";
    client->executeGET(bind2Url);

    auto currentLine = std::string();
    auto buf = std::vector<uint8_t>(128);

    int read;
    do {
        read = client->readFromSocket(buf.data(), buf.size());
        if (read > 0) {
            currentLine += std::string(buf.data(), buf.data() + read);
            while (currentLine.find("\n") != std::string::npos) {
                auto line = currentLine.substr(0, currentLine.find("\n"));
                currentLine = currentLine.substr(currentLine.find("\n") + 1,
                                                 currentLine.size());
                if (line.find("[[") != std::string::npos) {
                    auto event = std::make_unique<YouTubeEvent>(line.substr(1));
                    this->eventBus->postEvent(std::move(event));
                }
            }
        }
    } while (read >= 0);
}

void YouTubeLeanHandler::registerBindings(
    std::shared_ptr<berry::VmState> berry) {
    berry->export_this("yt_send_bind_data", this,
                       &YouTubeLeanHandler::sendBindData);
}

void YouTubeLeanHandler::registerPairCode(const std::string &pairCode) {
    auto newClient = std::make_shared<bell::HTTPClient>();
    auto bodyRequest =
        "&access_type=permanent&screen_id=" + this->screenId +
        "&device_id=" + this->uuid +
        "&screen_name=Euphonium&app=lb-v4&pairing_code=" + pairCode;
    newClient->executePOST(
        "https://www.youtube.com/api/lounge/pairing/register_pairing_code",
        bodyRequest, "application/x-www-form-urlencoded");
}

void YouTubeLeanHandler::sendBindData(std::string data) {
    auto newClient = std::make_shared<bell::HTTPClient>();
    std::string url = baseBindUrl + "&SID=" + SID +
                      "&gsessionid=" + gconnectId +
                      "&RID=1337&type=xmlhttp&CI=0&AID=3";
    newClient->executePOST(url, data, "application/x-www-form-urlencoded");
}
