// Copyright (c) Kuba Szczodrzyński 2022-1-16.

#include "WebmContainer.h"
#include "AudioCodecs.h"
#include "BellUtils.h"
#include "WebmElements.h"

using namespace bell;

#define BLOCK_LACING_MASK 0b110

WebmContainer::~WebmContainer() {
	freeAndNull((void *&)docType);
	freeAndNull((void *&)codecId);
	freeAndNull((void *&)codecPrivate);
	freeAndNull((void *&)cues);
	freeAndNull((void *&)sampleData);
	freeAndNull((void *&)laceSizes);
}

void WebmContainer::feed(const std::shared_ptr<bell::ByteStream> &stream, uint32_t position) {
	BaseContainer::feed(stream, position);
	freeAndNull((void *&)laceSizes);
	laceLeft = 0;
	readOutFrameSize = 0;
}

bool WebmContainer::parse() {
	bool segmentFound = false;
	do {
		readElem();
		switch (eid) {
			case ElementId::EBML:
				continue;
			case ElementId::DocType:
				docType = static_cast<char *>(malloc(esize + 1));
				docType[esize] = '\0';
				readBytes((uint8_t *)docType, esize);
				break;
			case ElementId::Segment:
				segmentFound = true;
				parseSegment(pos); // webm can only have a single segment
				break;
			default:
				skipBytes(esize);
		}
	} while (!segmentFound);
	if (strncmp(codecId, "A_OPUS", 6) == 0)
		codec = AudioCodec::OPUS;
	else if (strncmp(codecId, "A_VORBIS", 8) == 0)
		codec = AudioCodec::VORBIS;
	else
		codec = AudioCodec::UNKNOWN;
	isSeekable = cuesLen;
	return isParsed && codec != AudioCodec::UNKNOWN;
}

int32_t WebmContainer::getLoadingOffset(uint32_t timeMs) {
	if (!isSeekable || !cuesLen)
		return SAMPLE_NOT_SEEKABLE;
	auto offset = (int32_t)cues[0].offset;
	auto reqTime = (uint32_t)((float)timeMs * 1000000.0f / timescale);
	for (CuePoint *cue = cues + 1; cue < cues + cuesLen; cue++) {
		if (reqTime <= cue->time)
			return offset;
		offset = (int32_t)cue->offset;
	}
	return offset;
}

bool WebmContainer::seekTo(uint32_t timeMs) {
	auto reqTime = (uint32_t)((float)timeMs * 1000000.0f / timescale);
	if (reqTime <= currentTime)
		return false;
	// seeking ¯\_(ツ)_/¯
	readOutFrameSize = readCluster(reqTime);
	return !closed;
}

int32_t WebmContainer::getCurrentTimeMs() {
	return (int32_t)((float)currentTime * timescale / 1000000.0f);
}

uint8_t *WebmContainer::readSample(uint32_t &len) {
	if (readOutFrameSize)
		return readFrame(readOutFrameSize, len);
	if (laceLeft && laceLeft--)
		return readFrame(*(laceCurrent++), len);
	return readFrame(readCluster(), len);
}

uint32_t WebmContainer::readCluster(uint32_t untilTime) {
	uint32_t end;
	do {
		readElem();
		end = pos + esize;
		switch (eid) {
			case ElementId::Cluster:
				continue;
			case ElementId::Timestamp:
				clusterTime = readUint(esize);
				break;
			case ElementId::BlockGroup:
				continue;
			case ElementId::Block:
			case ElementId::SimpleBlock:
				if (readVarNum32() != audioTrackId) {
					skipTo(end);
					continue;
				}
				currentTime = clusterTime + readUint16();
				if (!untilTime || currentTime >= untilTime)
					return readBlock(end);
				skipTo(end); // skip all unneeded frames
				break;
			default:
				skipBytes(esize);
		}
	} while (!closed);
	return 0;
}

uint32_t WebmContainer::readBlock(uint32_t end) {
	uint8_t lacing = readUint8() & BLOCK_LACING_MASK;
	// https://www.matroska.org/technical/basics.html#simpleblock-structure
	if (!lacing) // no lacing (0b000)
		return end - pos;
	// use lacing
	laceLeft = readUint8() + 1;
	freeAndNull((void *&)laceSizes);
	laceSizes = static_cast<uint32_t *>(malloc(laceLeft * sizeof(uint32_t)));
	auto *size = laceSizes;

	for (uint8_t i = 0; i < laceLeft; i++) {
		if (lacing == 0b010) { // Xiph lacing (0b010)
			uint8_t sizeByte = readUint8();
			*size = sizeByte;
			while (sizeByte == 255) {
				sizeByte = readUint8();
				*size += sizeByte;
			}
		} else if (lacing == 0b110) { // EBML lacing (0b110)
			*size = readVarNum32();
		} else { // fixed-size lacing (0b100)
			*size = (end - pos) / laceLeft;
		}
		size++;
	}
	laceCurrent = laceSizes + 1;
	laceLeft--;
	return laceSizes[0];
}

uint8_t *WebmContainer::readFrame(uint32_t size, uint32_t &outLen) {
	if (!size)
		return nullptr;
	if (size > sampleLen) {
		free(sampleData);
		sampleData = static_cast<uint8_t *>(malloc(size));
		sampleLen = size;
	}
	outLen = readBytes(sampleData, size);
	readOutFrameSize = 0;
	return sampleData;
}

uint8_t *WebmContainer::getSetupData(uint32_t &len, AudioCodec matchCodec) {
	if (codec != matchCodec)
		return nullptr;
	len = codecPrivateLen;
	return codecPrivate;
}
