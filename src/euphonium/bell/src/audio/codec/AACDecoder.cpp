// Copyright (c) Kuba Szczodrzyński 2022-1-12.

#include "AACDecoder.h"

AACDecoder::AACDecoder() {
	aac = AACInitDecoder();
	pcmData = (int16_t *)malloc(AAC_MAX_NSAMPS * AAC_MAX_NCHANS * sizeof(int16_t));
}

AACDecoder::~AACDecoder() {
	AACFreeDecoder(aac);
	free(pcmData);
}

bool AACDecoder::setup(uint32_t sampleRate, uint8_t channelCount, uint8_t bitDepth) {
	frame.sampRateCore = (int)sampleRate;
	frame.nChans = channelCount;
	frame.bitsPerSample = bitDepth;
	return AACSetRawBlockParams(aac, 0, &frame) == 0;
}

uint8_t *AACDecoder::decode(uint8_t *inData, uint32_t inLen, uint32_t &outLen) {
	if (!inData)
		return nullptr;
	int status = AACDecode(
		aac,
		static_cast<unsigned char **>(&inData),
		reinterpret_cast<int *>(&inLen),
		static_cast<short *>(this->pcmData));
	AACGetLastFrameInfo(aac, &frame);
	if (status != ERR_AAC_NONE) {
		lastErrno = status;
		return nullptr;
	}
	outLen = frame.outputSamps * sizeof(int16_t);
	return (uint8_t *)pcmData;
}
