// Copyright (c) Kuba Szczodrzyński 2022-1-12.

#pragma once

#include "BaseContainer.h"

class BaseCodec {
  public:
	/**
	 * Setup the codec (sample rate, channel count, etc) using the specified container.
	 */
	virtual bool setup(BaseContainer *container);
	/**
	 * Setup the codec manually, using the provided values.
	 */
	virtual bool setup(uint32_t sampleRate, uint8_t channelCount, uint8_t bitDepth) = 0;
	/**
	 * Decode the given sample.
	 *
	 * @param [in] inData encoded data. Should allow nullptr, in which case nullptr should be returned.
	 * @param [in] inLen size of inData, in bytes
	 * @param [out] outLen size of output PCM data, in bytes
	 * @return pointer to decoded raw PCM audio data, allocated inside the codec object; nullptr on failure
	 */
	virtual uint8_t *decode(uint8_t *inData, uint32_t inLen, uint32_t &outLen) = 0;
	/**
	 * Read a single sample from the container, decode it, and return the result.
	 *
	 * @param [in] container media container to read the sample from (the container's codec must match this instance)
	 * @param [out] outLen size of output PCM data, in bytes
	 * @return pointer to decoded raw PCM audio data, allocated inside the codec object; nullptr on failure
	 */
	uint8_t *decode(BaseContainer *container, uint32_t &outLen);
	/**
	 * Last error that occurred, this is a codec-specific value.
	 * This may be set by a codec upon decoding failure.
	 */
	int lastErrno = -1;
};
