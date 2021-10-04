#ifndef EUPHONIUM_HTTP_EVENTS_H
#define EUPHONIUM_HTTP_EVENTS_H

#include <EventBus.h>
#include <memory.h>
#include <plugins/http/HTTPServer.h>

class HandleRouteEvent: public Event {
    public:
    HTTPRequest request;
    HandleRouteEvent(const HTTPRequest& request) {
        this->request = request;
        this->luaEventType = "handleRouteEvent";
    };

    sol::object toLua(lua_State* state) {
        return sol::make_object(
		     state, this->request);
    }
};

#endif