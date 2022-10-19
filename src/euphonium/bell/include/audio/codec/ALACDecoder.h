// Copyright (c) Kuba Szczodrzyński 2022-1-12.

#pragma once

#include "BaseCodec.h"
#include "alac_wrapper.h"

class ALACDecoder : public BaseCodec {
  private:
	alac_codec_s* alacCodec;
	int16_t *pcmData;

  public:
	ALACDecoder();
	~ALACDecoder();
	bool setup(uint32_t sampleRate, uint8_t channelCount, uint8_t bitDepth) override;
	uint8_t *decode(uint8_t *inData, uint32_t inLen, uint32_t &outLen) override;
};
