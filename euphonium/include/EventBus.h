#ifndef EUPHONIUM_EVENTBUS_H
#define EUPHONIUM_EVENTBUS_H

#include <memory>
#include <functional>
#include <queue>
#include <any>
#include <map>
#include <set>

enum class EventType : uint32_t {
    EVENT_FETCH_SERVICES,
};

struct Event {
    EventType eventType;
    std::any eventValue;
};

class EventSubscriber {
public:
    virtual ~EventSubscriber() = 0;
    virtual void handleEvent(std::unique_ptr<Event> event) = 0;
};

class EventBus {
private:
    std::queue<std::unique_ptr<Event>> eventQueue;
    std::map<EventType, std::set<std::unique_ptr<EventSubscriber>>> registeredListeners;
public:
    EventBus();
    void update();
    void postEvent(std::unique_ptr<Event> event);
    void addListener(EventType eventType, std::unique_ptr<EventSubscriber> eventSubscriber);
};

#endif