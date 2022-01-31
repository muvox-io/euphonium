#ifndef EUPHONIUM_CORE_EVENTS_H
#define EUPHONIUM_CORE_EVENTS_H

#include <EventBus.h>
#include <memory.h>

struct PlaybackInfo {
    std::string songName, albumName, artistName, sourceName, icon;
};

class SongChangedEvent : public Event {
  public:
    PlaybackInfo playbackInfo;
    SongChangedEvent(const std::string &songName, const std::string &albumName,
                     const std::string &artistName,
                     const std::string &sourceName, const std::string &icon) {
        this->playbackInfo = {.songName = songName,
                              .albumName = albumName,
                              .artistName = artistName,
                              .sourceName = sourceName,
                              .icon = icon};
        this->subType = "songChangedEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["songName"] = this->playbackInfo.songName;
        result["albumName"] = this->playbackInfo.albumName;
        result["artistName"] = this->playbackInfo.artistName;
        result["sourceName"] = this->playbackInfo.sourceName;
        result["icon"] = this->playbackInfo.icon;
        return result;
    }
};

class HookEvent : public Event {
  private:
    std::string hook;

  public:
    HookEvent(std::string hook) {
        this->hook = hook;
        this->subType = "hookEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["hook"] = this->hook;
        return result;
    }
};

class PauseChangedEvent : public Event {
  private:
    bool isPaused;

  public:
    PauseChangedEvent(bool isPaused) {
        this->isPaused = isPaused;
        this->subType = "statusChangedEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["isPaused"] = this->isPaused;
        return result;
    }
};

class AudioTakeoverEvent : public Event {
  private:
    std::string source;

  public:
    AudioTakeoverEvent(std::string source) {
        this->source = source;
        this->subType = "audioTakeoverEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["source"] = this->source;
        return result;
    }
};

class PlaybackError : public Event {
  private:
    std::string errorMessage;
    std::string source;

  public:
    PlaybackError(const std::string& source, const std::string& errorMessage) {
        this->source = source;
        this->errorMessage = errorMessage;
        this->subType = "playbackError";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["source"] = this->source;
        result["errorMessage"] = this->errorMessage;
        return result;
    }
};

class VolumeChangedEvent : public Event {
  private:
    int volume;

  public:
    VolumeChangedEvent(int volume) {
        this->volume = volume;
        this->subType = "volumeChangedEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["volume"] = this->volume;
        return result;
    }
};

#endif
