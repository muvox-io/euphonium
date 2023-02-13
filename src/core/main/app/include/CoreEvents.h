#pragma once

#include "BerryBind.h"
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
}  // namespace euph
