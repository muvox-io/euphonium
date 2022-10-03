#ifndef EUPHONIUM_HTTP_EVENTS_H
#define EUPHONIUM_HTTP_EVENTS_H

#include <EventBus.h>
#include <memory.h>
#include "Task.h"
#include <HTTPServer.h>

class HandleRouteEvent: public Event {
    public:
    std::unique_ptr<bell::HTTPRequest> request;
    HandleRouteEvent(std::unique_ptr<bell::HTTPRequest> request) {
        this->request = std::move(request);
        this->subType = "handleRouteEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["body"]  = this->request->body;
        result["connection"] = this->request->connection;
        result["handlerId"] = this->request->handlerId;
        result["queryParams"] = berry::to_map(this->request->queryParams);
        result["urlParams"] = berry::to_map(this->request->urlParams);
        return result;
    }
};

#endif
