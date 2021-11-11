#ifndef EUPHONIUM_CORE_EVENTS_H
#define EUPHONIUM_CORE_EVENTS_H

#include <EventBus.h>
#include <memory.h>

struct PlaybackInfo {
    std::string songName, albumName, artistName, sourceName;
};

class SongChangedEvent: public Event {
    public:
    PlaybackInfo playbackInfo;
    SongChangedEvent(std::string& songName, std::string& albumName, std::string& artistName, std::string& sourceName) {
        this->playbackInfo = {
            .songName = songName,
            .albumName = albumName,
            .artistName = artistName,
            .sourceName = sourceName
        };
        this->luaEventType = "songChangedEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    sol::object toLua(lua_State* state) {
        return sol::make_object(
		     state, this->playbackInfo);
    }
};

#endif