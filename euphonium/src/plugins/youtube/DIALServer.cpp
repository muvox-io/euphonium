#include "DIALServer.h"

const char *DEVICE_TEMPLATE_STR = R"(<?xml version="1.0" encoding="utf-8"?>
<root xmlns="urn:schemas-upnp-org:device-1-0" xmlns:r="urn:restful-tv-org:schemas:upnp-dd">
	<specVersion>
		<major>1</major>
		<minor>0</minor>
	</specVersion>
    <URLBase>http://192.168.1.208:80/dial</URLBase>
	<device>
		<deviceType>urn:schemas-upnp-org:device:dial:1</deviceType>
		<friendlyName>Euphonium</friendlyName>
		<manufacturer>-</manufacturer>
		<modelName>Euphonium</modelName>
		<UDN>uuid:%s</UDN>
	</device>
</root>)";

const char *APP_TEMPLATE_STR =
    R"(<service xmlns="urn:dial-multiscreen-org:schemas:dial">
                    <name>YouTube</name>
                    <options allowStop="true"/>
                    <state>stopped</state>
                </service>)";

void DIALServer::registerHandlers(std::shared_ptr<bell::HTTPServer> server) {
    BELL_LOG(info, "youtube", "Registering DIAL handlers");
    server->registerHandler(
        bell::RequestType::GET, "/ssdp/device-desc.xml",
        [this](bell::HTTPRequest &request) {;
            auto size = std::snprintf(nullptr, 0, DEVICE_TEMPLATE_STR,
                                      this->deviceId.c_str());
            std::string output(size + 1, '\0');
            std::sprintf(&output[0], DEVICE_TEMPLATE_STR,
                         this->deviceId.c_str());

            bell::HTTPResponse response = {
                .connectionFd = request.connection,
                .status = 200,
                .body = output.c_str(),
                .contentType = "application/xml",
                .extraHeaders = std::vector<std::string>(
                    {"Application-URL: http://192.168.1.208:80/dial/apps"}),

            };
            mainServer->respond(response);
        });

    server->registerHandler(
        bell::RequestType::GET, "/dial/apps/YouTube",
        [this](bell::HTTPRequest &request) {
            auto size =
                std::snprintf(nullptr, 0, DEVICE_TEMPLATE_STR, "stopped");
            std::string output(size + 1, '\0');
            std::sprintf(&output[0], APP_TEMPLATE_STR, "stopped");

            bell::HTTPResponse response = {
                .connectionFd = request.connection,
                .status = 200,
                .body = output.c_str(),
                .contentType = "application/xml; charset=utf-8",
            };
            mainServer->respond(response);
        });

    server->registerHandler(
        bell::RequestType::POST, "/dial/apps/YouTube",
        [this](bell::HTTPRequest &request) {
            this->leanHandler->registerPairCode(request.queryParams["pairingCode"]);

            bell::HTTPResponse response = {
                .connectionFd = request.connection,
                .status = 201,
                .body = "",
                .contentType = "application/xml; charset=utf-8",
            };
            mainServer->respond(response);
        });
}
