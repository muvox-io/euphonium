// Copyright (c) Kuba Szczodrzyński 2022-1-8.

#pragma once

#include "BaseContainer.h"
#include "Mpeg4Atoms.h"
#include <memory>

class Mpeg4Container : public BaseContainer {
  public:
	~Mpeg4Container();
	/**
	 * Start parsing the MP4 file. This method expects the source to read from 0th byte.
	 * This method leaves pos at first mdat data byte, or mdat header for fMP4 files.
	 */
	bool parse() override;
	int32_t getLoadingOffset(uint32_t timeMs) override;
	bool seekTo(uint32_t timeMs) override;
	int32_t getCurrentTimeMs() override;
	uint8_t *readSample(uint32_t &len) override;
	uint8_t *getSetupData(uint32_t &len, AudioCodec matchCodec) override;
	void feed(const std::shared_ptr<bell::ByteStream> &stream, uint32_t position) override;

  private:
	/**
	 * Parse a single movie fragment. This method expects the source to read moof data, without the header.
	 * After running, [pos] is left at next mdat header. A new fragment will be created if [pos] does not exist
	 * in [fragments] table.
	 */
	bool parseMoof(uint32_t moofSize);
	bool goToData();
	// char mediaBrand[5];
	uint32_t totalDuration;
	bool totalDurationPresent = false;
	int8_t audioTrackId = -1;
	uint32_t timescale = 0;
	uint32_t sampleSizeMax = 0;
	uint8_t *sampleData = nullptr;
	uint32_t sampleDataLen = 0;
	bool isParsed = false;
	bool isFragmented = false;
	/** True if source reads **audio** mdat data bytes, false if source reads atom headers */
	bool isInData = false;

  private: // data for the entire movie:
	/** All fragments in the MPEG file */
	Mpeg4Fragment *fragments;
	uint16_t fragmentsLen;
	/** Default sample descriptions for each track */
	SampleDefaults *sampleDefs;
	uint32_t sampleDefsLen;
	/** Track IDs of [sampleDef] items */
	uint32_t *sampleDefTracks;
	/** Sample Description Table */
	SampleDescription *sampleDesc;
	uint32_t sampleDescLen;

  private: // data changing every fragment:
	/** Chunks in the current fragment */
	Mpeg4ChunkRange *chunks;
	uint32_t chunksLen;
	/** Absolute chunk offsets in the current fragment */
	Mpeg4ChunkOffset *chunkOffsets;
	uint32_t chunkOffsetsLen;
	/** All sample descriptors in the current fragment */
	Mpeg4SampleRange *samples;
	uint32_t samplesLen;
	/** All sample sizes in the current fragment */
	Mpeg4SampleSize *sampleSizes;
	uint32_t sampleSizesLen;

  private: // current status and position within the file
	/** Currently loaded fragment (ptr) */
	Mpeg4Fragment *curFragment;
	/** The chunk currently being processed */
	Mpeg4Chunk curChunk;
	/** Size of the current sample (ptr) */
	Mpeg4SampleSize *curSampleSize;

  private: // Mpeg4Utils.cpp
	void readAtomHeader(uint32_t &size, uint32_t &type);
	void freeAll();
	void freeFragment();
	SampleDefaults *getSampleDef(uint32_t trackId);
	void setCurrentFragment();
	void setCurrentSample();
	static bool isInFragment(Mpeg4Fragment *f, uint32_t offset);
	static AudioCodec getCodec(SampleDescription *desc);
	Mpeg4Fragment *createFragment();
	int64_t findSample(int64_t byTime, int32_t byPos, uint64_t startTime);

  private: // Mpeg4Parser.cpp
	/** Populate [chunks] using the Sample-to-chunk Table */
	void readStsc();
	/** Populate [chunkOffsets] using the Chunk Offset Table */
	void readStco();
	/** Populate [samples] using the Time-to-sample Table */
	void readStts();
	/** Populate [sampleSizes] using the Sample Size Table */
	void readStsz();
	/** Populate [sampleDesc] using the Sample Description Table */
	void readStsd();

  private: // Mpeg4ParserFrag.cpp
	/** Populate [fragments] using the Segment Index Table */
	void readSidx(uint32_t atomSize);
	/** Populate [sampleDefs] using Track Extends */
	void readTrex();
	/** Populate [sampleDefs] using Track Fragment Header */
	void readTfhd(uint32_t trafEnd, uint32_t moofOffset);
	/** Populate [chunks, chunkOffsets, samples, sampleSizes] using Track Fragment Run Table */
	void readTrun(uint32_t atomSize, uint32_t moofOffset);
	void allocSampleData();
};
