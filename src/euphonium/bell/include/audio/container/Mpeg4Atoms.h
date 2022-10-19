// Copyright (c) Kuba Szczodrzyński 2022-1-8.

#pragma once

#include <cstdint>

enum class AudioSampleFormat;
enum class MP4AObjectType;
enum class MP4AProfile;

typedef struct {
	/** Absolute offset of mdat header (or moof for fMP4) */
	uint32_t start;
	/** Absolute offset of the last mdat byte */
	uint32_t end;
	/** Total duration of this fragment */
	uint32_t duration;
} Mpeg4Fragment;

typedef struct {
	/** Number of chunks this descriptor applies to */
	uint16_t count;
	/** Number of samples in the described chunks */
	uint32_t samples;
	uint16_t sampleDescriptionId;
} Mpeg4ChunkRange;

/** Absolute offset of the chunk data */
typedef uint32_t Mpeg4ChunkOffset;

typedef struct {
	/** Abs. offset of data start in the current chunk */
	uint32_t start;
	/** Abs. offset of data end in the current chunk */
	uint32_t end;
	/** Abs. offset of the next chunk data, or 0 for last chunk in a fragment */
	uint32_t nextStart;
} Mpeg4Chunk;

typedef struct {
	/** Number of samples this descriptor applies to */
	uint32_t count;
	/** Duration of the described samples */
	uint32_t duration;
} Mpeg4SampleRange;

/** Size of a single sample */
typedef uint32_t Mpeg4SampleSize;

/** Flags for a sample */
typedef uint32_t SampleFlags;

/** Default values for samples in the movie/fragment */
typedef struct {
	/** Absolute offset of first mdat byte */
	uint32_t offset;
	uint32_t sampleDescriptionId;
	uint32_t duration;
	uint32_t size;
	SampleFlags flags;
} SampleDefaults;

/** Sample Description Table */
typedef struct {
	uint16_t dataReferenceIndex;
	AudioSampleFormat format;
	// params for MPEG-4 Elementary Stream Descriptors
	MP4AObjectType mp4aObjectType;
	MP4AProfile mp4aProfile;
	// atom header for unknown descriptors
	uint32_t dataType;
	// codec-specific data (either DecoderSpecificInfo or the entire descriptor)
	uint32_t dataLength;
	uint8_t *data;
} SampleDescription;

typedef struct {
	// byte 1 - bits 0:7
	bool durationIsEmpty : 1;
	bool defaultBaseIsMoof : 1;
	bool dummy1 : 6;
	// byte 2 - bits 0:7
	uint8_t dummy2 : 8;
	// byte 3 - bits 0:7
	bool baseDataOffsetPresent : 1;
	bool sampleDescriptionIndexPresent : 1;
	bool dummy3 : 1;
	bool defaultSampleDurationPresent : 1;
	bool defaultSampleSizePresent : 1;
	bool defaultSampleFlagsPresent : 1;
	bool dummy4 : 2;
} TfFlags;

typedef struct {
	// byte 1 - bits 0:7
	uint8_t dummy1 : 8;
	// byte 2 - bits 0:7
	bool sampleDurationPresent : 1;
	bool sampleSizePresent : 1;
	bool sampleFlagsPresent : 1;
	bool sampleCompositionTimeOffsetsPresent : 1;
	bool dummy2 : 4;
	// byte 3 - bits 0:7
	bool dataOffsetPresent : 1;
	bool dummy3 : 1;
	bool firstSampleFlagsPresent : 1;
	bool dummy4 : 5;
} TrFlags;
