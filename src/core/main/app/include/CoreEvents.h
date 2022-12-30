#pragma once

#include "BerryBind.h"
#include "EventBus.h"

namespace euph {
class GenericVmEvent : public Event {
public:
  GenericVmEvent(std::string subType) {
    this->eventType = EventType::VM_MAIN_EVENT;
    this->subType = subType;
  }

  berry::map toBerry() override { return {}; }
};
}  // namespace euph
