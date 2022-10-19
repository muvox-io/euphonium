// Copyright (c) Kuba Szczodrzyński 2022-1-14.

#include "MP3Decoder.h"

MP3Decoder::MP3Decoder() {
	mp3 = MP3InitDecoder();
	pcmData = (int16_t *)malloc(MAX_NSAMP * MAX_NGRAN * MAX_NCHAN * sizeof(int16_t));
}

MP3Decoder::~MP3Decoder() {
	MP3FreeDecoder(mp3);
	free(pcmData);
}

bool MP3Decoder::setup(uint32_t sampleRate, uint8_t channelCount, uint8_t bitDepth) {
	return true;
}

uint8_t *MP3Decoder::decode(uint8_t *inData, uint32_t inLen, uint32_t &outLen) {
	if (!inData)
		return nullptr;
	int status = MP3Decode(
		mp3,
		static_cast<unsigned char **>(&inData),
		reinterpret_cast<int *>(&inLen),
		static_cast<short *>(this->pcmData),
		/* useSize */ 0);
	MP3GetLastFrameInfo(mp3, &frame);
	if (status != ERR_MP3_NONE) {
		lastErrno = status;
		return nullptr;
	}
	outLen = frame.outputSamps * sizeof(int16_t);
	return (uint8_t *)pcmData;
}
