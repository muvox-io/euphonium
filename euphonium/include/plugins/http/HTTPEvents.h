#ifndef EUPHONIUM_HTTP_EVENTS_H
#define EUPHONIUM_HTTP_EVENTS_H

#include <EventBus.h>
#include <memory.h>
#include "Task.h"
#include <HTTPServer.h>

class HandleRouteEvent: public Event {
    public:
    bell::HTTPRequest request;
    HandleRouteEvent(const bell::HTTPRequest& request) {
        this->request = request;
        this->subType = "handleRouteEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry_map toBerry() {
        berry_map result;
        result["body"]  = this->request.body;
        result["connection"] = this->request.connection;
        result["handlerId"] = this->request.handlerId;
        result["queryParams"] = to_berry_map(this->request.queryParams);
        result["urlParams"] = to_berry_map(this->request.urlParams);
        return result;
    }

    sol::object toLua(lua_State* state) {
        return sol::make_object(
		     state, this->request);
    }
};

#endif