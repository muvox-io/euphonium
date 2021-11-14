#ifndef EUPHONIUM_HTTP_EVENTS_H
#define EUPHONIUM_HTTP_EVENTS_H

#include <EventBus.h>
#include <memory.h>
#include <HTTPServer.h>

class HandleRouteEvent: public Event {
    public:
    bell::HTTPRequest request;
    HandleRouteEvent(const bell::HTTPRequest& request) {
        this->request = request;
        this->luaEventType = "handleRouteEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    sol::object toLua(lua_State* state) {
        return sol::make_object(
		     state, this->request);
    }
};

#endif