// Copyright (c) Kuba Szczodrzyński 2022-1-7.

#pragma once

#include "BinaryReader.h"
#include "ByteStream.h"
#include <cstdint>
#include <memory>

/**
 * Either the media file or the requested position/offset is not loaded yet.
 */
#define SAMPLE_NOT_LOADED	-1
/**
 * The media file does not contain the requested position/offset.
 */
#define SAMPLE_NOT_FOUND	-2
/**
 * The file is not seekable (i.e. doesn't contain an index table).
 */
#define SAMPLE_NOT_SEEKABLE -3

enum class AudioCodec;

class BaseContainer {
  public:
	BaseContainer() = default;
	/**
	 * Feed a new data source to the container.
	 * @param stream ByteStream reading source data
	 * @param position absolute position of the current ByteStream within the source media
	 */
	virtual void feed(const std::shared_ptr<bell::ByteStream> &stream, uint32_t position);
	/**
	 * Try to parse the media provided by the source stream.
	 * @return whether parsing was successful
	 */
	virtual bool parse() = 0;
	/**
	 * Get absolute offset within the source media for the given timestamp.
	 * When seeking to a specified time, the caller should run feed() with a stream
	 * reader starting at the returned offset. Depending on the container type,
	 * the returned offset may not point to the exact time position (i.e. chunks with
	 * headers), so seekTo() should be used afterwards.
	 *
	 * @param timeMs requested timestamp, in milliseconds
	 * @return byte offset within the source media that should be loaded
	 * in order to seek to the requested position; negative value on error
	 */
	virtual int32_t getLoadingOffset(uint32_t timeMs) = 0;
	/**
	 * Try to seek to the specified position (in milliseconds), using the currently
	 * loaded source stream. This method will fail if the source stream does not yield
	 * data for the requested position, or block until the stream loads data for this position.
	 *
	 * @param timeMs requested timestamp, in milliseconds
	 */
	virtual bool seekTo(uint32_t timeMs) = 0;
	/**
	 * Get the current playback position, in milliseconds. May return -1 if the track
	 * is not playing (has ended or not started yet).
	 */
	virtual int32_t getCurrentTimeMs() = 0;
	/**
	 * Read an encoded audio sample from the container, starting at the current position.
	 *
	 * @param [out] len length of the data stored in the returned pointer, in bytes
	 * @return pointer to data allocated inside the container object; should not be freed or changed.
	 * On failure, nullptr is returned, and len is left unchanged.
	 */
	virtual uint8_t *readSample(uint32_t &len) = 0;
	/**
	 * Get optional initialization data for the specified codec. This may be used by a codec,
	 * for containers that contain the setup data.
	 *
	 * @param [out] len length of the setup data
	 * @return ptr to [len] setup data bytes, or nullptr if not available/not supported
	 */
	virtual uint8_t *getSetupData(uint32_t &len, AudioCodec matchCodec) = 0;

  public:
	bool closed = false;
	bool isSeekable = false;
	// audio parameters
	AudioCodec codec = (AudioCodec)0;
	uint32_t sampleRate = 0;
	uint8_t channelCount = 0;
	uint8_t bitDepth = 0;
	uint32_t durationMs = 0;

  protected:
	std::unique_ptr<bell::BinaryReader> reader;
	std::shared_ptr<bell::ByteStream> source;
	uint32_t pos = 0;
	uint8_t readUint8();
	uint16_t readUint16();
	uint32_t readUint24();
	uint32_t readUint32();
	uint64_t readUint64();
	uint32_t readVarint32();
	uint32_t readBytes(uint8_t *dst, uint32_t num);
	uint32_t skipBytes(uint32_t num);
	uint32_t skipTo(uint32_t offset);
};
