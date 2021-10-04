#ifndef EUPHONIUM_EVENTBUS_H
#define EUPHONIUM_EVENTBUS_H

#include <memory>
#include <functional>
#include <queue>
#include <any>
#include <unordered_map>
#include <set>

enum class EventType : uint32_t {
    EVENT_FETCH_SERVICES,
    AUDIO_PACKET_EVENT
};

struct Event {
    EventType eventType;
    std::any eventValue;
};

class EventSubscriber {
public:
    virtual void handleEvent(std::unique_ptr<Event> event) = 0;
};

class EventBus {
private:
    std::queue<std::unique_ptr<Event>> eventQueue;
    std::unordered_map<EventType, std::vector<std::reference_wrapper<EventSubscriber>>> registeredListeners;
public:
    EventBus();
    void update();
    void postEvent(std::unique_ptr<Event> event);
    void addListener(EventType eventType, EventSubscriber& eventSubscriber);
};

#endif