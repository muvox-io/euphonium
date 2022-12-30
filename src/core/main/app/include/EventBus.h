#pragma once

#include <any>
#include <functional>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>

#include <BerryBind.h>
#include <WrappedSemaphore.h>

namespace euph {
enum class EventType : uint32_t {
  VM_MAIN_EVENT,
  VM_ERROR_EVENT,
  CONNECTIVITY_EVENT,
  WEBSOCKET_EVENT
};

class Event {
 public:
  Event(){};

  virtual ~Event(){};
  EventType eventType;
  std::string subType;
  virtual berry::map toBerry() = 0;
};

class EventSubscriber {
 public:
  virtual void handleEvent(std::unique_ptr<Event>& event) = 0;
};

class EventBus {
 private:
  std::queue<std::unique_ptr<Event>> eventQueue;
  std::unordered_map<EventType,
                     std::vector<std::reference_wrapper<EventSubscriber>>>
      registeredListeners;

 public:
  EventBus();
  std::unique_ptr<bell::WrappedSemaphore> eventSemaphore;
  bool update();
  void postEvent(std::unique_ptr<Event> event);
  void addListener(EventType eventType, EventSubscriber& eventSubscriber);
};
}  // namespace euph
