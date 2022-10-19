// Copyright (c) Kuba Szczodrzyński 2022-1-10.

#include "Mpeg4Container.h"

/** Populate [fragments] using the Segment Index Table */
void Mpeg4Container::readSidx(uint32_t atomSize) {
	// https://b.goeswhere.com/ISO_IEC_14496-12_2015.pdf (page 121)
	uint32_t offset = pos + atomSize;
	uint8_t version = readUint8();
	skipBytes(3);						// skip flags
	if (audioTrackId != readUint32()) { // check reference_ID
		skipBytes(atomSize - 8);		// skip the rest
		return;
	}
	skipBytes(4);				 // skip uint(32) timescale
	skipBytes(version ? 12 : 4); // skip zeroes, depending on version
	offset += readUint32();		 // read first_offset
	skipBytes(2);				 // skip uint(16) reserved

	fragmentsLen = readUint16();
	fragments = (Mpeg4Fragment *)malloc(fragmentsLen * sizeof(Mpeg4Fragment));
	for (uint32_t i = 0; i < fragmentsLen; i++) {
		auto size = readUint32();
		if (size & (1 << 31)) {
			skipBytes(8);
			continue; // ignore references to sidx, for now
		}
		fragments[i].start = offset;
		fragments[i].end = offset + size;
		fragments[i].duration = readUint32();
		offset += size;
		skipBytes(4); // skip SAP info
	}
	isFragmented = true;
}

/** Populate [sampleDefs] using Track Extends */
void Mpeg4Container::readTrex() {
	// https://b.goeswhere.com/ISO_IEC_14496-12_2015.pdf (page 69)
	skipBytes(4); // skip version and flags
	sampleDefsLen++;
	sampleDefs = (SampleDefaults *)realloc(sampleDefs, sampleDefsLen * sizeof(SampleDefaults));
	sampleDefTracks = (uint32_t *)realloc(sampleDefTracks, sampleDefsLen * sizeof(uint32_t));
	uint32_t i = sampleDefsLen - 1;
	sampleDefTracks[i] = readUint32();
	sampleDefs[i].offset = 0;
	sampleDefs[i].sampleDescriptionId = readUint32();
	sampleDefs[i].duration = readUint32();
	sampleDefs[i].size = readUint32();
	sampleDefs[i].flags = readUint32();
}

/** Populate [sampleDefs] using Track Fragment Header */
void Mpeg4Container::readTfhd(uint32_t trafEnd, uint32_t moofOffset) {
	skipBytes(1); // skip version
	TfFlags tfhdFlags = {};
	readBytes((uint8_t *)&tfhdFlags, 3);
	if (audioTrackId != readUint32()) {
		skipTo(trafEnd); // skip the rest of traf
		return;
	}
	auto *def = getSampleDef(audioTrackId);
	if (!def) {
		skipTo(trafEnd); // error?
		return;
	}
	def->offset = 0;
	if (tfhdFlags.baseDataOffsetPresent)
		def->offset = readUint64();
	if (tfhdFlags.sampleDescriptionIndexPresent)
		def->sampleDescriptionId = readUint32();
	if (tfhdFlags.defaultSampleDurationPresent)
		def->duration = readUint32();
	if (tfhdFlags.defaultSampleSizePresent)
		def->size = readUint32();
	if (tfhdFlags.defaultSampleFlagsPresent)
		def->flags = readUint32();
	if (tfhdFlags.defaultBaseIsMoof)
		def->offset += moofOffset;
}

/** Populate [chunks, chunkOffsets, samples, sampleSizes] using Track Fragment Run Table */
void Mpeg4Container::readTrun(uint32_t atomSize, uint32_t moofOffset) {
	skipBytes(1); // skip version
	TrFlags trunFlags = {};
	readBytes((uint8_t *)&trunFlags, 3);
	// audioTrackId is guaranteed to match this trun's track ID
	auto *def = getSampleDef(audioTrackId);
	if (!def) {
		skipBytes(atomSize - 4); // error?
		return;
	}
	uint32_t i, j = 0;
	uint32_t sampleCnt = readUint32();
	uint32_t offset = def->offset ? def->offset : moofOffset; // base offset is baseDataOffset or moofOffset
	// SampleFlags flags = def->flags;
	if (trunFlags.dataOffsetPresent)
		offset += readUint32();
	// if (trunFlags.firstSampleFlagsPresent)
	// 	flags = readUint32();

	// treat every trun as a single new chunk
	i = chunksLen++;
	chunks = (Mpeg4ChunkRange *)realloc(chunks, chunksLen * sizeof(Mpeg4ChunkRange));
	chunks[i].count = 1;
	chunks[i].samples = sampleCnt;
	chunks[i].sampleDescriptionId = def->sampleDescriptionId;
	i = chunkOffsetsLen++;
	chunkOffsets = (Mpeg4ChunkOffset *)realloc(chunkOffsets, chunkOffsetsLen * sizeof(Mpeg4ChunkOffset));
	chunkOffsets[i] = offset;

	// add all samples' sizes from this trun
	i = sampleSizesLen;
	sampleSizesLen += sampleCnt;
	sampleSizes = (Mpeg4SampleSize *)realloc(sampleSizes, sampleSizesLen * sizeof(Mpeg4SampleSize));
	// count duration changes for Mpeg4SampleRanges
	auto *durations = (uint32_t *)malloc(sampleCnt * sizeof(uint32_t));
	uint32_t prevDuration = 0, durationChanges = 0;

	// TODO optimize memory usage for when all samples are of equal sizes
	for (; i < sampleSizesLen; i++) {
		durations[j] = trunFlags.sampleDurationPresent ? readUint32() : def->duration;
		sampleSizes[i] = trunFlags.sampleSizePresent ? readUint32() : def->size;
		if (sampleSizes[i] > sampleSizeMax)
			sampleSizeMax = sampleSizes[i];
		if (trunFlags.sampleFlagsPresent)
			skipBytes(4); // skip flags, for now
		if (trunFlags.sampleCompositionTimeOffsetsPresent)
			skipBytes(4); // skip sample_composition_time_offset
		// count duration changes
		if (durations[j] != prevDuration) {
			prevDuration = durations[j];
			durationChanges++;
		}
		j++;
	}

	// add each duration change as a sample range
	i = samplesLen;
	samplesLen += durationChanges;
	samples = (Mpeg4SampleRange *)realloc(samples, samplesLen * sizeof(Mpeg4SampleRange));
	prevDuration = 0;
	uint32_t durationCnt = 0; // how many consecutive samples have this duration
	for (j = 0; j < sampleCnt; j++) {
		if (durations[j] != prevDuration) {
			if (prevDuration) {
				samples[i].count = durationCnt;
				samples[i].duration = prevDuration;
			}
			prevDuration = durations[j];
			durationCnt = 1;
		} else {
			durationCnt++;
		}
	}
	samples[samplesLen - 1].count = durationCnt;
	samples[samplesLen - 1].duration = prevDuration;
	// free temp array
	free(durations);
	// reallocate sampleData if the max size changes
	allocSampleData();
}
