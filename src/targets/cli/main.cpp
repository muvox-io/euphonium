#include <memory>
#include "BellHTTPServer.h"
#include "BellTask.h"
#include "BellUtils.h"
#include "Connectivity.h"
#include "Core.h"
#include "EuphLogger.h"
#include "DesktopAudioOutput.h"
#include "WrappedSemaphore.h"
#include "civetweb.h"

class FakeConnectivity : public euph::Connectivity, public bell::Task {
  std::string nextEvent = "";
  std::unique_ptr<bell::WrappedSemaphore> eventSemaphore;

 public:
  FakeConnectivity(std::shared_ptr<euph::EventBus> eventBus) : bell::Task("FakeConnectivity", 1024, 0, 0) {
    this->data = {
      euph::Connectivity::State::CONNECTED,
      euph::Connectivity::ConnectivityType::DEFAULT};
    this->eventSemaphore = std::make_unique<bell::WrappedSemaphore>(5);
    this->eventBus = eventBus;

    startTask();
  }
  ~FakeConnectivity() {}

  void displayNameLoaded(std::string& name) override {
  }

  void registerHandlers(std::shared_ptr<bell::BellHTTPServer> http) override {

    std::cout << "Registering handlers" << std::endl;

    http->registerGet("/wifi/scan", [this, &http](struct mg_connection* conn) {
      this->nextEvent = "scan";
      this->eventSemaphore->give();

      return http->makeJsonResponse("{ \"status\": \"ok\" }");
    });

    http->registerGet("/wifi/state", [this, &http](struct mg_connection* conn) {
      this->sendStateUpdate();

      return http->makeJsonResponse("{ \"status\": \"ok\" }");
    });

    http->registerPost(
        "/wifi/connect", [this, &http](struct mg_connection* conn) {
          auto connInfo = mg_get_request_info(conn);
          std::vector<uint8_t> body(connInfo->content_length);
          mg_read(conn, body.data(), connInfo->content_length);

          auto bodyJson = nlohmann::json::parse(body.begin(), body.end());

          if (bodyJson.find("ssid") == bodyJson.end() ||
              bodyJson.find("password") == bodyJson.end()) {
            return http->makeJsonResponse("{ \"status\": \"error\" }");
          }

          if (bodyJson["password"] == "miloslaw") {
            this->nextEvent = "connect";
            this->eventSemaphore->give();
          } else {

            this->nextEvent = "connect_fail";
            this->eventSemaphore->give();
          }

          return http->makeJsonResponse("{ \"status\": \"ok\" }");
        });
  }

  void runTask() override {
    BELL_SLEEP_MS(100);

    data.state = euph::Connectivity::State::CONNECTED;
    data.type = euph::Connectivity::ConnectivityType::WIFI_STA;

    this->sendStateUpdate();
    return;

    while (true) {
      this->eventSemaphore->wait();

      if (this->nextEvent == "scan") {
        data.jsonBody = "{ \"scanning\": true }";
        sendStateUpdate();

        BELL_SLEEP_MS(5000);
        auto networks = nlohmann::json{{"networks", nlohmann::json::array()}};

        networks["networks"].push_back(
            {{"ssid", "Crab_Free_Wifi"}, {"rssi", -50}, {"open", false}});
        networks["networks"].push_back(
            {{"ssid", "Interia 21-A"}, {"rssi", -50}, {"open", true}});
        networks["networks"].push_back(
            {{"ssid", "B-Rock SPOT"}, {"rssi", -50}, {"open", true}});
        networks["networks"].push_back(
            {{"ssid", "Crab_Free_Wifi-2.4G"}, {"rssi", -50}, {"open", false}});
        networks["networks"].push_back(
            {{"ssid", "SYRION7624"}, {"rssi", -50}, {"open", false}});

        data.jsonBody = networks.dump();
        data.state = euph::Connectivity::State::CONNECTED_NO_INTERNET;
      }

      if (this->nextEvent == "connect_fail") {
        data.state = euph::Connectivity::State::CONNECTING;
        sendStateUpdate();
        BELL_SLEEP_MS(5000);

        data.jsonBody = "{ \"error\": true }";
        data.state = euph::Connectivity::State::CONNECTED_NO_INTERNET;
      }

      if (this->nextEvent == "connect") {
        data.state = euph::Connectivity::State::CONNECTING;
        sendStateUpdate();

        BELL_SLEEP_MS(5000);

        data.state = euph::Connectivity::State::CONNECTED;
        data.type = euph::Connectivity::ConnectivityType::WIFI_AP;
        data.ipAddr = "192.168.1.112";
        data.ssid = "Crab_Free_Wifi";
        sendStateUpdate();
        BELL_SLEEP_MS(3000);
        data.type = euph::Connectivity::ConnectivityType::WIFI_STA;
        sendStateUpdate();
      }

      sendStateUpdate();
    }
  }
};

int main() {
  initializeEuphoniumLogger();
  auto eventBus = std::make_shared<euph::EventBus>();

  auto output = std::make_shared<euph::DesktopAudioOutput>();
  auto connectivity = std::make_shared<FakeConnectivity>(eventBus);
  auto core = std::make_unique<euph::Core>(connectivity, eventBus, output);

  core->handleEventLoop();

  while (true) {
    BELL_SLEEP_MS(1000);
  }
  return 0;
}
