
#ifndef EUPHONIUM_YT_EVENTS_H
#define EUPHONIUM_YT_EVENTS_H

#include <EventBus.h>
#include <memory.h>
#include <BerryBind.h>

class YouTubeEvent: public Event {
    public:
    std::string body;
    YouTubeEvent(std::string body) {
        this->body = body;
        this->subType = "youtubeEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["body"]  = this->body;
        return result;
    }
};

#endif
