// Copyright (c) Kuba Szczodrzyński 2022-1-12.

#include "BaseCodec.h"

bool BaseCodec::setup(BaseContainer *container) {
	return this->setup(container->sampleRate, container->channelCount, container->bitDepth);
}

uint8_t *BaseCodec::decode(BaseContainer *container, uint32_t &outLen) {
	uint32_t len;
	auto *data = container->readSample(len);
	return decode(data, len, outLen);
}
