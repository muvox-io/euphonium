#include "SnapcastPlugin.h"

using namespace euph;

SnapcastPlugin::SnapcastPlugin(std::shared_ptr<euph::Context> ctx)
    : bell::Task("snapcast", 16 * 1024, 0, 1) {
  this->ctx = ctx;
  startTask();
}

SnapcastPlugin::~SnapcastPlugin() {}

void SnapcastPlugin::runPlugin() {
}

std::string SnapcastPlugin::getName() {
  return "snapcast";
}

void SnapcastPlugin::shutdownAudio() {

}

void SnapcastPlugin::initializeBindings() {

}

void SnapcastPlugin::runTask() {
}
