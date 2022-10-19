#ifndef AUDIOSINK_H
#define AUDIOSINK_H

#include <cstdint>
#include <cstdlib>
#include <vector>

class AudioSink
{
  public:
	AudioSink() {}
	virtual ~AudioSink() {}
	virtual void feedPCMFrames(const uint8_t *buffer, size_t bytes) = 0;
	virtual void volumeChanged(uint16_t volume) {}
	// return true if the sink supports rate changing
	virtual bool setRate(uint16_t sampleRate) { return false; }
	bool softwareVolumeControl = true;
	bool usign = false;
};

#endif
