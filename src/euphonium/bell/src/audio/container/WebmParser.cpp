// Copyright (c) Kuba Szczodrzyński 2022-1-16.

#include "WebmContainer.h"
#include "WebmElements.h"

void WebmContainer::parseSegment(uint32_t start) {
	uint16_t cueIdx = 0;
	do {
		readElem();
		switch (eid) {
			case ElementId::Info:
			case ElementId::Tracks:
				continue;
			case ElementId::TimestampScale:
				timescale = (float)readUint(esize);
				break;
			case ElementId::Duration:
				durationMs = (uint32_t)(readFloat(esize) * timescale / 1000000.0f);
				break;
			case ElementId::TrackEntry:
				if (audioTrackId == 255)
					parseTrack(pos + esize);
				else // skip other tracks if audio is already found
					skipBytes(esize);
				break;
			case ElementId::Cues:
				// try to guess the amount of CuePoints from the total size:
				// - CuePoint = id(1) + size(1) + CueTime + CueTrackPositions
				// - CueTime = id(1) + size(1) + value(2) # avg. 16 bit
				// - CueTrackPositions = id(1) + size(1) + CueTrack + CueClusterPosition
				// - CueTrack = id(1) + size(1) + value(1)
				// - CueClusterPosition = id(1) + size(1) + value(3) # avg. 24 bit
				// total: approx. 16 bytes
				cuesLen += esize / 16;
				cues = static_cast<CuePoint *>(realloc(cues, cuesLen * sizeof(CuePoint)));
				continue; // read the next child
			case ElementId::CuePoint:
				if (cueIdx >= cuesLen) {
					cuesLen++;
					cues = static_cast<CuePoint *>(realloc(cues, cuesLen * sizeof(CuePoint)));
				}
				parseCuePoint(cueIdx++, pos + esize, start);
				break;
			case ElementId::Cluster:
				isParsed = audioTrackId != 255;
				clusterEnd = pos + esize;
				return;
			default:
				skipBytes(esize);
		}
	} while (!isParsed);
}

void WebmContainer::parseTrack(uint32_t end) {
	uint8_t trackId = 255;
	uint32_t trackRate = 0;
	uint8_t trackChannels = 0;
	uint8_t trackBits = 0;
	char *trackCodecId = nullptr;
	uint8_t *trackCodecPrivate = nullptr;
	uint32_t trackCodecPrivateLen = 0;
	do {
		readElem();
		switch (eid) {
			case ElementId::TrackNumber:
				trackId = readUint(esize);
				break;
			case ElementId::TrackType:
				if (readUint8() != 0x02) { // allow only audio tracks
					skipTo(end);
					return;
				}
				break;
			case ElementId::CodecID:
				trackCodecId = static_cast<char *>(malloc(esize + 1));
				trackCodecId[esize] = '\0';
				readBytes((uint8_t *)trackCodecId, esize);
				break;
			case ElementId::CodecPrivate:
				trackCodecPrivate = static_cast<uint8_t *>(malloc(esize));
				trackCodecPrivateLen = esize;
				readBytes(trackCodecPrivate, esize);
				break;
			case ElementId::Audio:
				continue;
			case ElementId::SamplingFrequency:
				trackRate = (uint32_t)readFloat(esize);
				break;
			case ElementId::Channels:
				trackChannels = readUint(esize);
				break;
			case ElementId::BitDepth:
				trackBits = readUint(esize);
				break;
			default:
				skipBytes(esize);
		}
	} while (pos < end);
	// not-audio tracks do not even get to this point
	audioTrackId = trackId;
	sampleRate = trackRate;
	channelCount = trackChannels;
	bitDepth = trackBits;
	codecId = trackCodecId;
	codecPrivate = trackCodecPrivate;
	codecPrivateLen = trackCodecPrivateLen;
}

void WebmContainer::parseCuePoint(uint16_t idx, uint32_t end, uint32_t segmentStart) {
	CuePoint *cue = cues + idx;
	do {
		readElem();
		switch (eid) {
			case ElementId::CueTime:
				cue->time = readUint(esize);
				break;
			case ElementId::CueTrackPositions:
				continue;
			case ElementId::CueClusterPosition:
				cue->offset = segmentStart + readUint(esize);
				break;
			default:
				skipBytes(esize);
		}
	} while (pos < end);
}
