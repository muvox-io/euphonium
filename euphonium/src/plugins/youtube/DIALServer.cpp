#include "DIALServer.h"

const char *DEVICE_TEMPLATE_STR = R"(<?xml version="1.0"?>
<root xmlns="urn:schemas-upnp-org:device-1-0" configId="1">
	<specVersion>
		<major>1</major>
		<minor>0</minor>
	</specVersion>
    <URLBase>http://localhost:80/youtube</URLBase>
	<device>
		<deviceType>urn:schemas-upnp-org:device:dial:1</deviceType>
		<friendlyName>Euphonium</friendlyName>
		<manufacturer>-</manufacturer>
		<modelDescription>Play the audio of YouTube videos</modelDescription>
		<modelName>Euphonium</modelName>
		<modelNumber>1.0.0</modelNumber>
		<UDN>uuid:%s</UDN>
<serviceList>
<service>
<serviceType>urn:dial-multiscreen-org:service:dial:1</serviceType>
<serviceId>urn:dial-multiscreen-org:serviceId:dial</serviceId>
<controlURL>/ssdp/notfound</controlURL>
<eventSubURL>/ssdp/notfound</eventSubURL>
<SCPDURL>/ssdp/notfound</SCPDURL>
</service>
</serviceList>
	</device>
</root>)";

const char *APP_TEMPLATE_STR = R"(<?xml version="1.0" encoding="UTF-8"?>
<service xmlns="urn:dial-multiscreen-org:schemas:dial" dialVer="1.7">
    <name>YouTube</name>
    <options allowStop="true"/>
    <state>%s</state>
</service>
)";

void DIALServer::registerHandlers(std::shared_ptr<bell::HTTPServer> server) {
    BELL_LOG(info, "youtube", "Registering DIAL handlers");
    server->registerHandler(
        bell::RequestType::GET, "/dial/upnp/device_desc.xml",
        [this](bell::HTTPRequest &request) {
        BELL_LOG(info, "youtube", "REQUESTED REEEEEE");
            auto size = std::snprintf(nullptr, 0, DEVICE_TEMPLATE_STR,
                                      this->deviceId.c_str());
            std::string output(size + 1, '\0');
            std::sprintf(&output[0], DEVICE_TEMPLATE_STR,
                         this->deviceId.c_str());

            bell::HTTPResponse response = {
                .connectionFd = request.connection,
                .status = 200,
                .body = output.c_str(),
                .contentType = "application/xml; charset=utf-8",
            };
            mainServer->respond(response);
        });

    server->registerHandler(
        bell::RequestType::GET, "/dial/apps/YouTube",
        [this](bell::HTTPRequest &request) {
            auto size = std::snprintf(nullptr, 0, DEVICE_TEMPLATE_STR,
                                      "stopped");
            std::string output(size + 1, '\0');
            std::sprintf(&output[0], APP_TEMPLATE_STR,
                         "stopped");

            bell::HTTPResponse response = {
                .connectionFd = request.connection,
                .status = 200,
                .body = output.c_str(),
                .contentType = "application/xml; charset=utf-8",
            };
            mainServer->respond(response);
        });
}
