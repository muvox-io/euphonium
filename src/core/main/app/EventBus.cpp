#include "EventBus.h"

using namespace euph;

EventBus::EventBus() {
  eventSemaphore = std::make_unique<bell::WrappedSemaphore>(5);
}

bool EventBus::update() {
  bool gotUpdate = false;
  while (!eventQueue.empty()) {
    gotUpdate = true;
    std::unique_ptr<Event> localPtr(std::move(eventQueue.front()));
    eventQueue.pop();
    if (localPtr == nullptr) {
      continue;
    }
    auto type = localPtr->eventType;
    auto it = registeredListeners.find(type);
    if (it != registeredListeners.end()) {
      for (auto&& subscriber : (*it).second) {
        subscriber.get().handleEvent(localPtr);
      }
    }
  }

  return gotUpdate;
}

void EventBus::postEvent(std::unique_ptr<Event> event) {
  eventQueue.push(std::move(event));
  eventSemaphore->give();
}

void EventBus::addListener(EventType eventType,
                           EventSubscriber& eventSubscriber) {
  registeredListeners[eventType].push_back(eventSubscriber);
}
