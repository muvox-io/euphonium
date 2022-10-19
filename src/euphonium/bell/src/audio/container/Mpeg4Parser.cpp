// Copyright (c) Kuba Szczodrzyński 2022-1-10.

#include "BellUtils.h"
#include "Mpeg4Container.h"
#include "Mpeg4Types.h"

using namespace bell;

/** Populate [chunks] using the Sample-to-chunk Table */
void Mpeg4Container::readStsc() {
	skipBytes(4); // skip version and flags
	chunksLen = readUint32();
	chunks = (Mpeg4ChunkRange *)malloc(chunksLen * sizeof(Mpeg4ChunkRange));
	for (uint32_t i = 0; i < chunksLen; i++) {
		chunks[i].count = readUint32();
		chunks[i].samples = readUint32();
		chunks[i].sampleDescriptionId = readUint32();
		if (i > 0) {
			chunks[i - 1].count = chunks[i].count - chunks[i - 1].count;
		}
	}
	if (chunkOffsetsLen) {
		chunks[chunksLen - 1].count = chunkOffsetsLen - chunks[chunksLen - 1].count + 1;
	}
}

/** Populate [chunkOffsets] using the Chunk Offset Table */
void Mpeg4Container::readStco() {
	skipBytes(4); // skip version and flags
	chunkOffsetsLen = readUint32();
	chunkOffsets = (Mpeg4ChunkOffset *)malloc(chunkOffsetsLen * sizeof(Mpeg4ChunkOffset));
	for (uint32_t i = 0; i < chunkOffsetsLen; i++) {
		chunkOffsets[i] = readUint32();
	}
	if (chunksLen) {
		chunks[chunksLen - 1].count = chunkOffsetsLen - chunks[chunksLen - 1].count + 1;
	}
}

/** Populate [samples] using the Time-to-sample Table */
void Mpeg4Container::readStts() {
	skipBytes(4); // skip version and flags
	samplesLen = readUint32();
	samples = (Mpeg4SampleRange *)malloc(samplesLen * sizeof(Mpeg4SampleRange));
	for (uint32_t i = 0; i < samplesLen; i++) {
		samples[i].count = readUint32();
		samples[i].duration = readUint32();
	}
}

/** Populate [sampleSizes] using the Sample Size Table */
void Mpeg4Container::readStsz() {
	skipBytes(4); // skip version and flags
	uint32_t sampleSize = readUint32();
	sampleSizesLen = readUint32();
	if (sampleSize) {
		sampleSizesLen = 1;
	}
	sampleSizes = (Mpeg4SampleSize *)malloc(sampleSizesLen * sizeof(Mpeg4SampleSize));
	if (sampleSize) {
		sampleSizes[0] = sampleSize;
		if (sampleSize > sampleSizeMax)
			sampleSizeMax = sampleSize;
		return;
	}
	for (uint32_t i = 0; i < sampleSizesLen; i++) {
		sampleSize = readUint32();
		if (sampleSize > sampleSizeMax)
			sampleSizeMax = sampleSize;
		sampleSizes[i] = sampleSize;
	}
	// reallocate sampleData if the max size changes
	allocSampleData();
}

/** Populate [sampleDesc] using the Sample Description Table */
void Mpeg4Container::readStsd() {
	// Helpful resources:
	// - STSD atom structure - ISO/IEC 14496-1 (page 277) - seems to cover QT desc ver.0
	// - ESDS atom structure - ISO/IEC 14496-1 (page 28)
	freeAndNull((void *&)sampleDesc);
	skipBytes(4); // skip version and flags
	sampleDescLen = readUint32();
	sampleDesc = (SampleDescription *)malloc(sampleDescLen * sizeof(SampleDescription));
	for (SampleDescription *desc = sampleDesc; desc < sampleDesc + sampleDescLen; desc++) {
		uint32_t entryEnd = readUint32() - 4 + pos;
		uint32_t esdsEnd = entryEnd;
		// https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap2/qtff2.html#//apple_ref/doc/uid/TP40000939-CH204-BBCHHGBH
		// General Structure of a Sample Description
		desc->format = (AudioSampleFormat)readUint32();
		desc->mp4aObjectType = MP4AObjectType::UNDEFINED;
		desc->mp4aProfile = MP4AProfile::UNDEFINED;
		skipBytes(6); // reserved
		desc->dataReferenceIndex = readUint16();
		// https://developer.apple.com/library/archive/documentation/QuickTime/QTFF/QTFFChap3/qtff3.html#//apple_ref/doc/uid/TP40000939-CH205-75770
		// Sound Sample Description (Version 0)
		uint16_t version = readUint16();
		skipBytes(6); // skip Revision level(2), Vendor(4)
		channelCount = readUint16();
		bitDepth = readUint16();
		skipBytes(4); // skip Compression ID(2), Packet size(2)
		sampleRate = readUint16();
		skipBytes(2); // decimal part of sample rate
		if (version >= 1) {
			// Sound Sample Description (Version 1)
			skipBytes(16); // skip Samples per packet(4), Bytes per packet(4), Bytes per frame(4), Bytes per sample(4)
		}
		// read the child atom
		uint32_t atomSize;
		AtomType atomType;
		readAtomHeader(atomSize, (uint32_t &)atomType);
		if (atomType == AtomType::ATOM_WAVE) {
			do {
				readAtomHeader(atomSize, (uint32_t &)atomType);
				if (atomType == AtomType::ATOM_ESDS) {
					esdsEnd = pos + atomSize;
					break;
				}
				skipBytes(atomSize);
			} while (pos < entryEnd);
			if (pos >= entryEnd) // something went wrong
				continue;
		}
		if (atomType != AtomType::ATOM_ESDS) {
			desc->dataType = (uint32_t)atomType;
			desc->dataLength = atomSize;
			desc->data = (uint8_t *)malloc(desc->dataLength);
			readBytes(desc->data, desc->dataLength);
			continue;
		}
		// read ESDS
		skipBytes(4); // skip esds flags
		while (pos < esdsEnd) {
			uint8_t tag = readUint8();
			uint32_t size = readVarint32();
			uint8_t flags;
			switch (tag) {
				case 0x03: // ES_Descriptor
					skipBytes(2);
					flags = readUint8();
					if (flags & 0b10000000)
						skipBytes(2);
					if (flags & 0b01000000)
						skipBytes(readUint8());
					if (flags & 0b00100000)
						skipBytes(2);
					break;
				case 0x04: // DecoderConfigDescriptor
					desc->mp4aObjectType = (MP4AObjectType)readUint8();
					skipBytes(12);
					break;
				case 0x05: // DecoderSpecificInfo
					if (desc->mp4aObjectType == MP4AObjectType::MP4A) {
						desc->mp4aProfile = (MP4AProfile)(readUint8() >> 3);
						skipBytes(size - 1);
					} else {
						desc->dataType = 0;
						desc->dataLength = size;
						desc->data = (uint8_t *)malloc(desc->dataLength);
						readBytes(desc->data, desc->dataLength);
					}
					break;
				default:
					skipBytes(size);
					break;
			}
		}
		// skip leftover atoms for version 1 QuickTime descriptors
		skipTo(entryEnd);
	}
}
