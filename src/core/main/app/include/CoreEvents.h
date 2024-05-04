#pragma once

#include <unordered_map>
#include "BerryBind.h"
#include "EmergencyMode.h"
#include "EventBus.h"

namespace euph {
class GenericVmEvent : public Event {
 public:
  GenericVmEvent(std::string subType) {
    this->eventType = EventType::VM_MAIN_EVENT;
    this->subType = subType;
  }

  berry::map toBerry() override { return {}; }
};

class VmRawCommandEvent : public Event {
 public:
  std::string command;
  VmRawCommandEvent(std::string command) {
    this->eventType = EventType::VM_RAW_COMMAND_EVENT;
    this->subType = "rawCommand";
    this->command = command;
  }

  berry::map toBerry() override { return {}; }
};

class ContextURIEvent : public Event {
 public:
  std::string uri;

  ContextURIEvent(std::string uri) {
    this->eventType = EventType::PLAYBACK_EVENT;
    this->subType = "contextURI";
    this->uri = uri;
  }

  berry::map toBerry() override {
    berry::map result;
    result["uri"] = this->uri;
    return result;
  }
};  // namespace euph

// Event used to notify the VM that the playback state has changed
class TrackInfoEvent : public Event {
 public:
  struct TrackInfo {
    std::string uri;
    std::string title;
    std::string artist;
    std::string album;
    std::string iconURL;

    bool canPlay = false;
    bool dontCache = false;
  };

  TrackInfo info;

  TrackInfoEvent(TrackInfo info) {
    this->eventType = EventType::VM_MAIN_EVENT;
    this->subType = "trackInfo";
    this->info = info;
  }

  berry::map toBerry() override {
    berry::map result;
    result["uri"] = this->info.uri;
    result["title"] = this->info.title;
    result["artist"] = this->info.artist;
    result["album"] = this->info.album;
    result["iconUrl"] = this->info.iconURL;
    result["canPlay"] = this->info.canPlay;
    result["source"] = this->info.uri.substr(0, this->info.uri.find(':'));
    result["dontCache"] = this->info.dontCache;
    return result;
  }
};  // namespace euph

class TrackHashChangeEvent : public Event {
 public:
  size_t hash;

  TrackHashChangeEvent(size_t hash) {
    this->eventType = EventType::PLAYBACK_EVENT;
    this->subType = "trackHashChange";
    this->hash = hash;
  }

  berry::map toBerry() override {
    berry::map result;
    result["trackHash"] = this->hash;
    return result;
  }
};  // namespace euph
class AudioVolumeEvent : public Event {
 public:
  enum VolumeSource { LOCAL, REMOTE };

  uint8_t volume = 0;
  VolumeSource source;

  AudioVolumeEvent(uint8_t volume, VolumeSource source) {
    this->eventType = EventType::VM_MAIN_EVENT;
    this->subType = "volume";
    this->volume = volume;
    this->source = source;
  }

  berry::map toBerry() override {
    berry::map result;

    result["value"] = (int)this->volume;
    result["source"] = "remote";

    if (this->source == VolumeSource::LOCAL) {
      result["source"] = "local";
    }

    return result;
  }
};

class PlaybackStateEvent : public Event {
 public:
  enum State { LOADING, PLAYING, PAUSED, STOPPED };

  const std::unordered_map<State, std::string> stateToString = {
      {State::LOADING, "loading"},
      {State::PLAYING, "playing"},
      {State::PAUSED, "paused"},
      {State::STOPPED, "queue_empty"},
  };

  State playbackState;

  PlaybackStateEvent(State state) {
    this->eventType = EventType::VM_MAIN_EVENT;
    this->subType = "playbackState";
    this->playbackState = state;
  }

  berry::map toBerry() override {
    berry::map result;

    // Map the enum to a string
    result["state"] = this->stateToString.at(this->playbackState);

    return result;
  }
};

class NotificationEvent : public Event {
 public:
  enum Type { ERROR, SUCCESS, WARNING, INFO };

  const std::unordered_map<Type, std::string> typeToString = {
      {Type::ERROR, "error"},
      {Type::SUCCESS, "success"},
      {Type::WARNING, "warning"},
      {Type::INFO, "info"},
  };

  Type notificationType;
  std::string message, submessage, source;

  NotificationEvent(Type type, std::string source, std::string message,
                    std::string submessage) {
    this->eventType = EventType::VM_MAIN_EVENT;
    this->subType = "notification";
    this->notificationType = type;
    this->message = message;
    this->source = source;
    this->submessage = submessage;
  }

  berry::map toBerry() override {
    berry::map result;

    // Map the enum to a string
    result["type"] = this->typeToString.at(this->notificationType);
    result["message"] = this->message;
    result["source"] = this->source;
    result["submessage"] = this->submessage;

    return result;
  }
};

class EmergencyModeTrippedEvent : public Event {
 public:
  EmergencyModeReason reason;

  EmergencyModeTrippedEvent(EmergencyModeReason reason) {
    this->eventType = EventType::EMERGENCY_MODE;
    this->subType = "tripped";
    this->reason = reason;
  }

  berry::map toBerry() override {
    berry::map result;
    result["reason"] = (int)this->reason;
    result["reason_str"] = EmergencyMode::getReasonString(this->reason);
    return result;
  }
};

}  // namespace euph
