#pragma once
#include "BellTask.h"
#include "EventBus.h"
#include "CoreEvents.h"

namespace euph {
class ManufacuringShell : public bell::Task {

 public:
  class VmManufacuringShellEvent : public Event {
   private:
    std::string body;

   public:
    VmManufacuringShellEvent(const std::string& body) {
      this->eventType = EventType::VM_MAIN_EVENT;
      this->subType = "manufacuring_shell";
      this->body = body;
    }
    ~VmManufacuringShellEvent(){};

    berry::map toBerry() override {
      berry::map m;
      m["body"] = this->body;
      return m;
    }
  };
  ManufacuringShell(std::shared_ptr<euph::EventBus> eventBus);
  void runTask() override;

 private:
  std::shared_ptr<euph::EventBus> eventBus;
  const char* TAG = "ManufacuringShell";
};
}  // namespace euph
