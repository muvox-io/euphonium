#include "HTTPDispatcher.h"


using namespace euph;

HTTPDispatcher::HTTPDispatcher(std::shared_ptr<euph::Context> ctx) {
  this->ctx = ctx;
  this->responseSemaphore = std::make_unique<bell::WrappedSemaphore>(0);
  this->server = std::make_unique<bell::BellHTTPServer>(8080);
}

HTTPDispatcher::~HTTPDispatcher() {}

void HTTPDispatcher::initialize() {
  EUPH_LOG(info, TAG, "Registering HTTP handlers");
  this->server->registerGet("/", [this](struct mg_connection* conn) {
    return this->server->makeJsonResponse("{\"status\": \"ok\"}");
  });
}

void HTTPDispatcher::setupBindings() {}

std::string HTTPDispatcher::_readBody(int connId) {
  return "";
}

size_t HTTPDispatcher::_readContentLength(int connId) {
  return 0;
}

void HTTPDispatcher::_registerHandler(int httpMethod, std::string path,
                                      int handlerId) {}

void HTTPDispatcher::_writeResponse(int connId, std::string body,
                                    std::string contentType, int statusCode) {}
