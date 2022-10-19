// Copyright (c) Kuba Szczodrzyński 2022-1-10.

#include "AudioCodecs.h"
#include "BellUtils.h"
#include "Mpeg4Container.h"
#include "Mpeg4Types.h"

using namespace bell;

void Mpeg4Container::readAtomHeader(uint32_t &size, uint32_t &type) {
	size = readUint32() - 8;
	type = readUint32();
}

void Mpeg4Container::allocSampleData() {
	if (sampleSizeMax != sampleDataLen) {
		freeAndNull((void *&)sampleData);
		sampleData = (uint8_t *)realloc(sampleData, sampleSizeMax);
		sampleDataLen = sampleSizeMax;
	}
}

void Mpeg4Container::freeAll() {
	freeAndNull((void *&)fragments);
	fragmentsLen = 0;
	freeAndNull((void *&)sampleDefs);
	freeAndNull((void *&)sampleDefTracks);
	sampleDefsLen = 0;
	for (SampleDescription *desc = sampleDesc; desc < sampleDesc + sampleDefsLen; desc++) {
		free(desc->data);
	}
	freeAndNull((void *&)sampleDesc);
	sampleDescLen = 0;
	freeAndNull((void *&)sampleData);
	freeFragment();
}

void Mpeg4Container::freeFragment() {
	freeAndNull((void *&)chunks);
	chunksLen = 0;
	freeAndNull((void *&)chunkOffsets);
	chunkOffsetsLen = 0;
	freeAndNull((void *&)samples);
	samplesLen = 0;
	freeAndNull((void *&)sampleSizes);
	sampleSizesLen = 0;
}

SampleDefaults *Mpeg4Container::getSampleDef(uint32_t trackId) {
	for (uint32_t i = 0; i < sampleDefsLen; i++) {
		if (sampleDefTracks[i] == trackId)
			return sampleDefs + i;
	}
	return nullptr;
}

bool Mpeg4Container::isInFragment(Mpeg4Fragment *f, uint32_t offset) {
	return offset >= f->start && offset < f->end;
}

void Mpeg4Container::setCurrentFragment() {
	if (!isParsed)
		return;
	for (Mpeg4Fragment *f = fragments; f < fragments + fragmentsLen; f++) {
		if (isInFragment(f, pos)) {
			curFragment = f;
			return;
		}
	}
	curFragment = nullptr;
}

void Mpeg4Container::setCurrentSample() {
	if (!isParsed)
		return;
	Mpeg4ChunkRange *chunk = chunks;
	Mpeg4ChunkOffset *chunkOffset = chunkOffsets;
	uint32_t chunkCnt = chunk->count;
	uint32_t chunkSampleCnt = chunk->samples;
	curChunk.start = 0;
	curChunk.end = 0;
	uint32_t offset = *chunkOffset;
	Mpeg4SampleSize *ss = sampleSizes;
	while (ss < sampleSizes + sampleSizesLen) {
		// for (Mpeg4SampleSize *ss = sampleSizes; ss < sampleSizes + sampleSizesLen; ss++) {
		offset += *ss;
		if (!curChunk.start && pos < offset) {				 // sample found
			curChunk.start = chunkOffset ? *chunkOffset : 0; // set chunk beginning
			curSampleSize = ss;								 // store reference to current sample
		}
		chunkSampleCnt--;	   // decrease remaining samples in chunk
		if (!chunkSampleCnt) { // no more samples
			chunkOffset++;	   // get next chunk offset
			if (chunkOffset >= chunkOffsets + chunkOffsetsLen)
				chunkOffset = nullptr;
			if (curChunk.start) {									 // chunk ended and beginning already found
				curChunk.end = offset;								 // set chunk end
				curChunk.nextStart = chunkOffset ? *chunkOffset : 0; // set next chunk offset
				break;
			}
			if (chunkOffset)
				offset = *chunkOffset;
			chunkCnt--;							 // decrease remaining chunks in range
			if (!chunkCnt) {					 // no more chunks
				chunk++;						 // get next chunk range
				if (chunk >= chunks + chunksLen) // something is not ok
					return;						 // -> fail
				chunkCnt = chunk->count;		 // update new chunk count from range
			}
			chunkSampleCnt = chunk->samples; // update new sample count from range
		}
		if (sampleSizesLen > 1)
			ss++;
	}
	isInData = pos >= curChunk.start && pos < curChunk.end;
}

Mpeg4Fragment *Mpeg4Container::createFragment() {
	uint32_t i = fragmentsLen++;
	fragments = (Mpeg4Fragment *)realloc(fragments, fragmentsLen * sizeof(Mpeg4Fragment));
	fragments[i].start = pos - 8;
	uint32_t fragmentEnd = chunkOffsets[chunkOffsetsLen - 1];
	uint32_t lastRangeSamples = chunks[chunksLen - 1].samples;
	if (sampleSizesLen == 1)
		fragmentEnd += *sampleSizes * lastRangeSamples;
	else {
		for (uint32_t j = sampleSizesLen - lastRangeSamples; j < sampleSizesLen; j++) {
			fragmentEnd += sampleSizes[j];
		}
	}
	fragments[i].end = fragmentEnd;
	fragments[i].duration = 0;
	totalDurationPresent = false;
	return fragments + i;
}

AudioCodec Mpeg4Container::getCodec(SampleDescription *desc) {
	switch (desc->format) {
		case AudioSampleFormat::OPUS:
			return AudioCodec::OPUS;
		case AudioSampleFormat::FLAC:
			return AudioCodec::FLAC;
		case AudioSampleFormat::MP4A:
			switch (desc->mp4aObjectType) {
				case MP4AObjectType::AAC_LC:
					return AudioCodec::AAC;
				case MP4AObjectType::OPUS:
					return AudioCodec::OPUS;
				case MP4AObjectType::VORBIS:
					return AudioCodec::VORBIS;
				case MP4AObjectType::MPEG1:
					return AudioCodec::MP3;
				case MP4AObjectType::MP4A:
					switch (desc->mp4aProfile) {
						case MP4AProfile::AAC_LC:
							return AudioCodec::AAC;
						case MP4AProfile::LAYER_3:
							return AudioCodec::MP3;
						default:
							return AudioCodec::UNKNOWN;
					}
				default:
					return AudioCodec::UNKNOWN;
			}
		default:
			return AudioCodec::UNKNOWN;
	}
}

int64_t Mpeg4Container::findSample(int64_t byTime, int32_t byPos, uint64_t startTime) {
	Mpeg4ChunkRange *chunk = chunks;
	Mpeg4SampleRange *sample = samples;
	Mpeg4ChunkOffset *chunkOffset = chunkOffsets;
	Mpeg4SampleSize *sampleSize = sampleSizes;
	uint32_t chunkCnt = chunk->count;
	uint32_t chunkSampleCnt = chunk->samples;
	uint32_t sampleRangeCnt = sample->count;

	uint64_t timeAbs = startTime;
	uint32_t offsetAbs = *chunkOffset;
	while (sampleSize < sampleSizes + sampleSizesLen) {
		if (byTime >= 0 && byTime <= timeAbs) {
			return offsetAbs;
		}
		if (byPos >= 0 && byPos <= offsetAbs) {
			return (int64_t)timeAbs;
		}
		timeAbs += sample->duration;
		sampleRangeCnt--;
		if (!sampleRangeCnt) {
			sample++;
			if (sample > samples + samplesLen)
				return SAMPLE_NOT_FOUND;
			sampleRangeCnt = sample->count;
		}
		chunkSampleCnt--;
		if (!chunkSampleCnt) {
			chunkCnt--;
			chunkOffset++;
			if (chunkOffset > chunkOffsets + chunkOffsetsLen)
				return SAMPLE_NOT_FOUND;
			offsetAbs = *chunkOffset;
			if (!chunkCnt) {
				chunk++;
				if (chunk > chunks + chunksLen)
					return SAMPLE_NOT_FOUND;
				chunkCnt = chunk->count;
			}
			chunkSampleCnt = chunk->samples;
		} else {
			offsetAbs += sampleSizesLen > 1 ? *(sampleSize++) : *sampleSize;
		}
	}
	return SAMPLE_NOT_FOUND;
}
