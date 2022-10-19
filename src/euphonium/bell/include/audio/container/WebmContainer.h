// Copyright (c) Kuba Szczodrzyński 2022-1-16.

#pragma once

#include "BaseContainer.h"

enum class ElementId;

class WebmContainer : public BaseContainer {
  public:
	~WebmContainer();
	bool parse() override;
	int32_t getLoadingOffset(uint32_t timeMs) override;
	bool seekTo(uint32_t timeMs) override;
	int32_t getCurrentTimeMs() override;
	uint8_t *readSample(uint32_t &len) override;
	uint8_t *getSetupData(uint32_t &len, AudioCodec matchCodec) override;
	void feed(const std::shared_ptr<bell::ByteStream> &stream, uint32_t position) override;

  private:
	typedef struct {
		uint32_t time;
		uint32_t offset;
	} CuePoint;

  private:
	// used while parsing
	uint32_t esize;
	ElementId eid;
	// container parameters
	char *docType = nullptr;
	uint8_t audioTrackId = 255;
	float timescale = 0.0f;
	char *codecId = nullptr;
	uint8_t *codecPrivate = nullptr;
	uint32_t codecPrivateLen = 0;
	// container state
	CuePoint *cues = nullptr;
	uint16_t cuesLen = 0;
	uint32_t clusterEnd = 0;
	uint32_t clusterTime = 0;
	uint32_t currentTime = 0;
	bool isParsed = false;
	// buffer
	uint8_t *sampleData = nullptr;
	uint32_t sampleLen = 0;
	// lacing parameters
	uint32_t *laceSizes = nullptr;
	uint32_t *laceCurrent = nullptr;
	uint8_t laceLeft = 0;
	// set to read the current buffer instead of loading new frames
	uint16_t readOutFrameSize = 0;

  private:
	uint32_t readVarNum32(bool raw = false);
	uint64_t readVarNum64();
	uint32_t readUint(uint8_t len);
	uint64_t readUlong(uint8_t len);
	float readFloat(uint8_t len);
	void readElem();
	void parseSegment(uint32_t start);
	void parseTrack(uint32_t end);
	void parseCuePoint(uint16_t idx, uint32_t end, uint32_t segmentStart);
	/**
	 * Continue reading elements until a block is encountered.
	 *
	 * If [untilTime] is set, the method will keep reading until [currentTime]
	 * is less than [untilTime]. Because of how WebM works, [pos] will be one frame later
	 * than the requested time, although the container will report the correct position.
	 *
	 * @return size of the frame pointed by [pos]
	 */
	uint32_t readCluster(uint32_t untilTime = 0);
	/**
	 * Parse a single block within a cluster. This method will populate lacing parameters if needed.
	 * @param end offset of the next byte after this block
	 * @return size of the frame pointed by [pos]
	 */
	uint32_t readBlock(uint32_t end);
	uint8_t *readFrame(uint32_t size, uint32_t &outLen);
};
