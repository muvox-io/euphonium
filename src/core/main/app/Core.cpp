#include "Core.h"

using namespace euph;

Core::Core() {
    bell::setDefaultLogger();
    BELL_LOG(info, TAG, "Core created");
}

Core::~Core() {
}