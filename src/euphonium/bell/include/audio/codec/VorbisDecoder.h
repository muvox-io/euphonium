// Copyright (c) Kuba Szczodrzyński 2022-1-14.

#pragma once

#include "BaseCodec.h"
#include "ivorbiscodec.h"

class VorbisDecoder : public BaseCodec {
  private:
	vorbis_info *vi = nullptr;
	vorbis_comment *vc = nullptr;
	vorbis_dsp_state *vd = nullptr;
	ogg_packet op = {};
	int16_t *pcmData;

  public:
	VorbisDecoder();
	~VorbisDecoder();
	bool setup(uint32_t sampleRate, uint8_t channelCount, uint8_t bitDepth) override;
	uint8_t *decode(uint8_t *inData, uint32_t inLen, uint32_t &outLen) override;
	bool setup(BaseContainer *container) override;

  private:
	void setPacket(uint8_t *inData, uint32_t inLen) const;
};
