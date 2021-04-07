#ifndef EUPHONIUM_EVENTBUS_H
#define EUPHONIUM_EVENTBUS_H

#include <memory>
#include <functional>
#include <queue>
#include <any>
#include <map>

enum class EventType : uint32_t {
    EVENT_FETCH_SERVICES,
};

struct Event {
    EventType eventType;
    std::any eventValue;
};

typedef std::function<void(std::unique_ptr<Event>)> eventListener;

class EventBus {
private:
    std::queue<std::unique_ptr<Event>> eventQueue;
    std::map<uint32_t, eventListener&> registeredListeners;
public:
    EventBus();
    void update();
    void postEvent(std::unique_ptr<Event> event);
    void addListener(EventType eventType, eventListener& eventListener);
};

#endif