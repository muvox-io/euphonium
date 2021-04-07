#include "ConfigurationApiService.h"
#include "HTTPServer.h"
#include "JSON.h"


void ConfigurationApiService::runConfigurationServer()
{
    // auto server = std::make_shared<HTTPServer>(2137);
    // httpHandler handlerService = [coreRef](HTTPRequest &request) {
    //     // fetch requested service
    //     auto serviceName = request.urlParams["name"];
    //     auto service = coreRef->getServiceWithName(serviceName);

    //     // Generate config json
    //     std::ostringstream jsonBody;
    //     writeJSON(jsonBody, service->getConfigurationRef());

    //     // prepare response
    //     HTTPResponse res;
    //     res.status = 200;
    //     res.body = jsonBody.str();
    //     return res;
    // };

    // httpHandler handlerKon = [coreRef](HTTPRequest &request) {
    //     // prepare response
    //     HTTPResponse res;
    //     res.status = 200;
    //     res.body = "<img src=\"https://cdn.pixabay.com/photo/2018/08/17/01/14/horse-3611921__340.jpg\"/>";
    //     return res;
    // };

    // server->registerHandler(RequestType::GET, "/service/:name", handlerService);
    // server->registerHandler(RequestType::GET, "/horse", handlerKon);
    // server->listen();
}