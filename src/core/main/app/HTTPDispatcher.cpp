#include "HTTPDispatcher.h"
#include <fmt/core.h>
#include <fstream>
#include <memory>
#include <string_view>
#include "BellHTTPServer.h"
#include "BellTar.h"
#include "BellUtils.h"
#include "CoreEvents.h"
#include "EventBus.h"
#include "MDNSService.h"
#include "MGStreamAdapter.h"
#include "civetweb.h"
#include "ghc_filesystem.h"

using namespace euph;

HTTPDispatcher::HTTPDispatcher(std::shared_ptr<euph::Context> ctx) {
  this->ctx = ctx;
  this->responseSemaphore = std::make_unique<bell::WrappedSemaphore>(
      MAX_CONNECTION_BINDS + 1);  // a safe value

// TODO: Handle it properly
#ifdef ESP_PLATFORM
  port = 80;
#endif

  this->server = std::make_shared<bell::BellHTTPServer>(port);
  auto subscriber = static_cast<EventSubscriber*>(this);
  this->ctx->eventBus->addListener(EventType::CONNECTIVITY_EVENT, *subscriber);
}

HTTPDispatcher::~HTTPDispatcher() {}

void HTTPDispatcher::handleEvent(std::unique_ptr<Event>& event) {
  if (event->eventType == EventType::CONNECTIVITY_EVENT) {
    auto connectivityEvent =
        static_cast<Connectivity::ConnectivityEvent*>(event.get());
    auto connectivityData = connectivityEvent->data;

    // Update the connectivity type
    this->isRunningAPMode = ctx->connectivity->getData().type ==
                            Connectivity::ConnectivityType::WIFI_AP;
  }
}

void HTTPDispatcher::initialize() {
  EUPH_LOG(debug, TAG, "Registering HTTP handlers");

  // Assign connectivity type
  this->isRunningAPMode = ctx->connectivity->getData().type ==
                          Connectivity::ConnectivityType::WIFI_AP;

  // Handle captive portal redirect
  this->server->registerNotFound([this](struct mg_connection* conn) {
    BELL_LOG(debug, TAG, "Redirecting to captive portal");
    mg_printf(conn,
              "HTTP/1.1 302 Temporary Redirect\r\nContent-Type: text/html"
              "\r\nLocation: /\r\nAccess-Control-Allow-Origin: "
              "*\r\nConnection: close\r\n\r\n"
              "Redirect to the captive portal");
    return this->server->makeEmptyResponse();
  });

  this->server->registerGet("/web/**", [this](struct mg_connection* conn) {
    std::scoped_lock lock(webAccessMutex);
    this->serveWeb(conn);
    return this->server->makeEmptyResponse();
  });

  this->server->registerGet("/assets/**", [this](struct mg_connection* conn) {
    std::scoped_lock lock(webAccessMutex);
    this->serveWeb(conn);
    return this->server->makeEmptyResponse();
  });

  this->server->registerGet("/", [this](struct mg_connection* conn) {
    std::scoped_lock lock(webAccessMutex);
    this->serveWeb(conn);
    return this->server->makeEmptyResponse();
  });

  this->server->registerWS(
      "/repl",
      [this](const struct mg_connection* conn, char* data, size_t dataSize) {
        // Convert the data to a string
        std::string dataString(data, dataSize);

        // Post the event to the event bus
        auto event = std::make_unique<VmRawCommandEvent>(dataString);
        this->ctx->eventBus->postEvent(std::move(event));
      },
      [this](const struct mg_connection* conn,
             bell::BellHTTPServer::WSState state) {
        switch (state) {
          case bell::BellHTTPServer::WSState::READY: {
            EUPH_LOG(debug, TAG, "REPL websocket connection open");
            std::scoped_lock lock(this->websocketConnectionsMutex);
            // Drop constness here, because civetweb calls callbacks with const pointers
            // but we need to store the connection for later use
            this->replWebsocketConnections.push_back(
                (struct mg_connection*)conn);
            break;
          }
          case bell::BellHTTPServer::WSState::CLOSED: {
            EUPH_LOG(debug, TAG, "REPL websocket connection closed");
            std::scoped_lock lock(this->websocketConnectionsMutex);
            // remove the connection from the list
            this->replWebsocketConnections.erase(
                find(replWebsocketConnections.begin(),
                     replWebsocketConnections.end(), conn));
            break;
          }
          default:
            break;
        }
      });

  this->server->registerWS(
      "/events",
      [this](struct mg_connection* conn, char* data, size_t dataSize) {
        if (data == nullptr || dataSize == 0)
          return;
        // Convert the data to a string
        std::string dataString(data, dataSize);

        // Post the event to the event bus
        auto event = std::make_unique<VmWebsocketEvent>(dataString);
        this->ctx->eventBus->postEvent(std::move(event));
      },
      [this](const struct mg_connection* conn,
             bell::BellHTTPServer::WSState state) {
        switch (state) {
          case bell::BellHTTPServer::WSState::READY: {
            EUPH_LOG(debug, TAG, "Websocket connection open");
            std::scoped_lock lock(this->websocketConnectionsMutex);
            // Drop constness here, because civetweb calls callbacks with const pointers
            // but we need to store the connection for later use
            this->websocketConnections.push_back((struct mg_connection*)conn);
            break;
          }
          case bell::BellHTTPServer::WSState::CLOSED: {
            EUPH_LOG(debug, TAG, "Websocket connection closed");
            std::scoped_lock lock(this->websocketConnectionsMutex);
            // remove the connection from the list
            this->websocketConnections.erase(find(websocketConnections.begin(),
                                                  websocketConnections.end(),
                                                  conn));
            break;
          }
          default:
            break;
        }
      });
}

std::shared_ptr<bell::BellHTTPServer> HTTPDispatcher::getServer() {
  return this->server;
}

bool HTTPDispatcher::strEndsWith(const std::string& fullString,
                                 const std::string& suffix) {
  if (fullString.length() >= suffix.length()) {
    return (0 == fullString.compare(fullString.length() - suffix.length(),
                                    suffix.length(), suffix));
  } else {
    return false;
  }
}

bool HTTPDispatcher::tryToServeFile(struct mg_connection* conn,
                                    std::string path) {
  // Prefix of package in the fs
  std::string webPrefix =
      fmt::format("{}/pkgs/web/dist/{}", ctx->rootPath, path);

  std::string contentType = "text/plain";

  // Deduce content-type
  if (strEndsWith(webPrefix, ".html")) {
    contentType = "text/html";
  } else if (strEndsWith(webPrefix, ".js")) {
    contentType = "application/javascript";
  } else if (strEndsWith(webPrefix, ".css")) {
    contentType = "text/css";
  }

  // Frontend is packed into .gz files, that are served with gzip encoding, even if client requests path without .gz
  bool isGzip = false;

  auto assetFile = std::ifstream(webPrefix);
  if (!assetFile.is_open()) {
    // If the file doesn't exist, try to check if a .gz version exists
    webPrefix = fmt::format("{}/pkgs/web/dist/{}.gz", ctx->rootPath, path);
    assetFile = std::ifstream(webPrefix);
    isGzip = true;
  }

  if (!assetFile.is_open()) {
    return false;
  }

  // Get the file size
  assetFile.seekg(0, std::ios::end);
  auto fileSize = assetFile.tellg();
  assetFile.seekg(0, std::ios::beg);

  // Write the headers
  std::string extraHeaders = isGzip ? "Content-Encoding: gzip\r\n" : "";
  mg_printf(conn,
            "HTTP/1.1 %d OK\r\nContent-Type: "
            "%s\r\n%sContent-Length: %d\r\nConnection: close\r\n\r\n",
            200, contentType.c_str(), extraHeaders.c_str(), (int)fileSize);

  // Transfer the file
  MGStreamAdapter streamAdapter(conn);
  streamAdapter << assetFile.rdbuf();
  streamAdapter.flush();

  return true;
}

void HTTPDispatcher::serveWeb(struct mg_connection* conn) {
  const char* hostHeader = mg_get_header(conn, "Host");
  if (hostHeader != nullptr && this->isRunningAPMode) {
    // Redirect all hosts that differ from the default one to the default one
    if (std::string_view(hostHeader).find(CAPTIVE_DEFAULT_HOST) ==
        std::string::npos) {
      mg_printf(conn,
                "HTTP/1.1 302 Temporary Redirect\r\nContent-Type: text/html"
                "\r\nLocation: http://%s/\r\nAccess-Control-Allow-Origin: "
                "*\r\nConnection: close\r\n\r\n"
                "Redirect to the captive portal",
                CAPTIVE_DEFAULT_HOST);
      return;
    }
  }

  auto reqInfo = mg_get_request_info(conn);
  std::string_view uri = std::string_view(reqInfo->local_uri);

  std::string filePath = "index.html";

  // Substract / from assets path
  if (uri.find("/assets/") != std::string::npos) {
    filePath = uri.substr(1, uri.size());
  }

  EUPH_LOG(info, TAG, "Web access URI %s", filePath.c_str());
  bool found = this->tryToServeFile(conn, filePath);

  if (!found) {
    mg_printf(conn,
              "HTTP/1.1 404 Not found\r\nContent-Type: text/html"
              "\r\nConnection: close\r\n\r\n"
              "404 not found");
  }
}

void HTTPDispatcher::setupBindings() {
  EUPH_LOG(debug, TAG, "Setting up HTTP-scripting bindings");
  ctx->vm->export_this("_register_handler", this,
                       &HTTPDispatcher::_registerHandler, "http");
  ctx->vm->export_this("_read_body", this, &HTTPDispatcher::_readBody, "http");
  ctx->vm->export_this("_read_content_length", this,
                       &HTTPDispatcher::_readContentLength, "http");
  ctx->vm->export_this("_write_response", this, &HTTPDispatcher::_writeResponse,
                       "http");
  ctx->vm->export_this("_write_tar_response", this,
                       &HTTPDispatcher::_writeTarResponse, "http");
  ctx->vm->export_this("_read_route_params", this,
                       &HTTPDispatcher::_readRouteParams, "http");
  ctx->vm->export_this("_extract_tar", this, &HTTPDispatcher::_extractTar,
                       "http");
  ctx->vm->export_this("_broadcast_websocket", this,
                       &HTTPDispatcher::_broadcastWebsocket, "http");
  ctx->vm->export_this("_register_mdns", this, &HTTPDispatcher::_registerMDNS,
                       "http");

  ctx->vm->stdoutCallback = [this](const char* buffer, size_t size) {
    std::scoped_lock lock(this->websocketConnectionsMutex);

    // Redirect berry's stdout to all connected websockets in the REPL endpoint
    for (auto&& conn : this->replWebsocketConnections) {
      mg_websocket_write(conn, MG_WEBSOCKET_OPCODE_TEXT, buffer, size);
    }
  };
}

std::string HTTPDispatcher::_readBody(int connId) {
  auto conn = this->bindConnections[connId];
  const struct mg_request_info* reqInfo = mg_get_request_info(conn);
  std::string body(reqInfo->content_length, ' ');

  mg_read(conn, body.data(), reqInfo->content_length);
  return body;
}

size_t HTTPDispatcher::_readContentLength(int connId) {
  auto conn = this->bindConnections[connId];
  const struct mg_request_info* reqInfo = mg_get_request_info(conn);
  return reqInfo->content_length;
}

void HTTPDispatcher::_registerHandler(int httpMethod, std::string path,
                                      int handlerId) {
  HTTPDispatcher::Method method =
      static_cast<HTTPDispatcher::Method>(httpMethod);
  switch (method) {
    case HTTPDispatcher::Method::GET:
      this->server->registerGet(
          path, [this, handlerId](struct mg_connection* conn) {
            std::scoped_lock lock(httpRequestMutex);
            this->bindConnections[this->nextBindId] = conn;
            // Prepare a response event to the scripting layer
            auto event = std::make_unique<HTTPDispatcher::VmEvent>(
                handlerId, this->nextBindId);

            this->ctx->eventBus->postEvent(std::move(event));

            this->nextBindId = (this->nextBindId + 1) % MAX_CONNECTION_BINDS;
            this->responseSemaphore->twait(HTTP_RESPONSE_TIMEOUT);

            return this->server->makeEmptyResponse();
          });
      break;
    case HTTPDispatcher::Method::POST:
      this->server->registerPost(
          path, [this, handlerId](struct mg_connection* conn) {
            std::scoped_lock lock(httpRequestMutex);
            this->bindConnections[this->nextBindId] = conn;

            // Prepare a response event to the scripting layer
            auto event = std::make_unique<HTTPDispatcher::VmEvent>(
                handlerId, this->nextBindId);
            this->ctx->eventBus->postEvent(std::move(event));

            this->nextBindId = (this->nextBindId + 1) % MAX_CONNECTION_BINDS;
            this->responseSemaphore->twait(HTTP_RESPONSE_TIMEOUT);

            return this->server->makeEmptyResponse();
          });
      break;
  }
}

void HTTPDispatcher::broadcastWebsocket(const std::string& body) {
  std::scoped_lock lock(this->websocketConnectionsMutex);

  // for each in this->websocketConnections
  for (auto&& conn : this->websocketConnections) {
    mg_websocket_write(conn, MG_WEBSOCKET_OPCODE_TEXT, body.data(),
                       body.size());
  }
}

void HTTPDispatcher::_broadcastWebsocket(std::string body) {
  this->broadcastWebsocket(body);
}

void HTTPDispatcher::_writeResponse(int connId, std::string body,
                                    std::string contentType, int statusCode) {
  auto conn = this->bindConnections[connId];

  mg_printf(conn,
            "HTTP/1.1 %d OK\r\nContent-Type: "
            "%s\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n",
            statusCode, contentType.c_str());
  mg_write(conn, body.c_str(), body.size());
  this->responseSemaphore->give();
}

void HTTPDispatcher::_writeTarResponse(int connId, std::string sourcePath,
                                       std::string filename) {
  auto conn = this->bindConnections[connId];

  mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/x-tar\r\n"
            "Content-Disposition: attachment; filename=\"%s\"\r\n\r\n",
            filename.c_str());

  MGStreamAdapter streamAdapter(conn);
  bell::BellTar::writer writer(streamAdapter);
  //TODO: support directories
  auto dirPath = fmt::format("{}{}/", ctx->rootPath, sourcePath);

  for (const auto& entry : ghc::filesystem::directory_iterator(dirPath)) {
    if (entry.is_directory()) {
      continue;
    }

    std::vector<char> fileBuffer;
    std::ifstream fileStream(entry.path());

    std::copy(std::istream_iterator<char>(fileStream),
              std::istream_iterator<char>(), std::back_inserter(fileBuffer));

    writer.put(entry.path().filename(),
               reinterpret_cast<char*>(fileBuffer.data()), fileBuffer.size());
  }
  writer.finish();

  this->responseSemaphore->give();
}

void HTTPDispatcher::_extractTar(int connId, std::string dstPath) {
  auto conn = this->bindConnections[connId];

  MGInputStreamAdapter streamAdapter(conn);
  bell::BellTar::reader reader(streamAdapter);
  std::string path = fmt::format("{}{}", ctx->rootPath, dstPath);
  EUPH_LOG(debug, TAG, "Extracting tar to %s", path.c_str());
  reader.extract_all_files(path);
}

void HTTPDispatcher::_registerMDNS(std::string name, std::string type,
                                   std::string proto, berry::map txt) {
  std::map<std::string, std::string> txtMap;

  // Convert berry::map to std::map, because the mdns library requires it
  for (auto&& [key, value] : txt) {
    if (std::any_cast<std::string>(&value)) {
      txtMap[key] = std::any_cast<std::string>(value);
    }
  }

  MDNSService::registerService(name, type, proto, "", port, txtMap);
}

berry::map HTTPDispatcher::_readRouteParams(int connId) {
  auto conn = this->bindConnections[connId];
  auto params = bell::BellHTTPServer::extractParams(conn);

  berry::map result;
  for (auto&& param : params) {
    result[param.first] = param.second;
  }

  return result;
}
