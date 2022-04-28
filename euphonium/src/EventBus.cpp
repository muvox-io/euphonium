#include "EventBus.h"

EventBus::EventBus()
{
    // this->registeredListeners = std::map<EventType, std::set<eventListener*>>();
    eventSemaphore = std::make_unique<WrappedSemaphore>(5);
}

bool EventBus::update()
{
    bool gotUpdate = false;
    while (!eventQueue.empty())
    {
        gotUpdate = true;
        std::unique_ptr localPtr(std::move(eventQueue.front()));
        eventQueue.pop();
        auto type = localPtr->eventType;
        auto it = registeredListeners.find(type);
        if (it != registeredListeners.end())
        {
            for (auto &&subscriber : (*it).second)
            {
                subscriber.get().handleEvent(std::move(localPtr));
            }
        }
    }

    return gotUpdate;
}

void EventBus::postEvent(std::unique_ptr<Event> event)
{
    eventQueue.push(std::move(event));
    eventSemaphore->give();
}

void EventBus::addListener(EventType eventType, EventSubscriber& eventSubscriber)
{
    registeredListeners[eventType].push_back(eventSubscriber);
}
