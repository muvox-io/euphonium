#include "SnapcastPlugin.h"
#include <mutex>

using namespace euph;

SnapcastPlugin::SnapcastPlugin(std::shared_ptr<euph::Context> ctx)
    : bell::Task("snapcast", 16 * 1024, 0, 1) {
  this->ctx = ctx;
  startTask();
}

SnapcastPlugin::~SnapcastPlugin() {}

void SnapcastPlugin::_connect(std::string url, int port) {
  try {
    snapcastConnection =
        std::make_unique<snapcast::Connection>(this->ctx->displayName);
    snapcastConnection->connectWithServer(url, port);
    this->isRunning = true;
    // this->startTask();
  } catch (std::exception& e) {
    EUPH_LOG(error, TASK, "Error connecting to snapcast server: %s", e.what());
    snapcastConnection = nullptr;
  }
}

void SnapcastPlugin::_disconnect() {
  if (snapcastConnection != nullptr) {
    this->isRunning = false;
    std::scoped_lock lock(this->runningMutex);
    snapcastConnection = nullptr;
  }
}

std::string SnapcastPlugin::getName() {
  return "snapcast";
}

void SnapcastPlugin::shutdownAudio() {
  _disconnect();
}

void SnapcastPlugin::initializeBindings() {
  this->ctx->vm->export_this("_connect", this, &SnapcastPlugin::_connect,
                             "snapcast");
  this->ctx->vm->export_this("_disconnect", this, &SnapcastPlugin::_disconnect,
                             "snapcast");
}

void SnapcastPlugin::runTask() {
  std::scoped_lock lock(this->runningMutex);
  while (this->isRunning && this->snapcastConnection != nullptr) {
    snapcastConnection->handleUpdate(ctx);
  }
}
