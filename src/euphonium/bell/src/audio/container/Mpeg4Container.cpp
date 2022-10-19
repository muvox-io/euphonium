// Copyright (c) Kuba Szczodrzyński 2022-1-8.

#include "Mpeg4Container.h"
#include "AudioCodecs.h"
#include "Mpeg4Atoms.h"
#include "Mpeg4Types.h"

Mpeg4Container::~Mpeg4Container() {
	freeAll();
	this->source->close();
}

void Mpeg4Container::feed(const std::shared_ptr<bell::ByteStream> &stream, uint32_t position) {
	BaseContainer::feed(stream, position);
	if (isParsed) {
		// this is needed to support seeking backwards, as goToData() always moves forward only
		setCurrentFragment();
		isInData = false;
		goToData();
	}
}

bool Mpeg4Container::parse() {
	freeAll();
	if (pos != 0) {
		isParsed = false;
		return false;
	}
	uint32_t size;
	AtomType type;
	uint32_t moovEnd = 0, mdiaEnd = 0;
	bool parsed = false, error = false, hasMoov = false, hasHdlr = false;
	while (!parsed && !error && !closed) {
		readAtomHeader(size, (uint32_t &)type);
		switch (type) {
				/*case AtomType::ATOM_FTYP:
					readBytes(mediaBrand, 4);
					mediaBrand[4] = '\0';
					skipBytes(size - 4);
					break;*/

			case AtomType::ATOM_MVEX:
			case AtomType::ATOM_TRAK:
			case AtomType::ATOM_MINF:
			case AtomType::ATOM_STBL:
				// this causes the next iteration to read the next direct child atom
				continue;
			case AtomType::ATOM_MOOV:
				moovEnd = pos + size;
				hasMoov = true;
				continue;
			case AtomType::ATOM_MDIA:
				mdiaEnd = pos + size;
				continue;

			case AtomType::ATOM_TREX:
				readTrex();
				break;
			case AtomType::ATOM_TKHD:
				skipBytes(12);
				if (audioTrackId == -1) {
					audioTrackId = (int8_t)readUint32();
					skipBytes(size - 16);
				} else {
					// new track header, but audio track already found
					skipTo(moovEnd);
				}
				break;
			case AtomType::ATOM_MDHD:
				skipBytes(12);
				timescale = readUint32();
				totalDuration = readUint32();
				totalDurationPresent = true;
				durationMs = totalDuration * 1000LL / timescale;
				if (!sampleRate)
					sampleRate = timescale;
				hasHdlr = false;
				skipBytes(size - 20);
				break;
			case AtomType::ATOM_HDLR:
				if (hasHdlr) {
					skipBytes(size);
					continue;
				}
				hasHdlr = true;
				skipBytes(8);
				if (readUint32() != (uint32_t)AtomType::ATOM_SOUN) {
					skipTo(mdiaEnd);   // skip the rest of mdia atom
					audioTrackId = -1; // unset the track ID, so the next tkhd can set it
				} else {
					skipBytes(size - 12);
				}
				break;
			case AtomType::ATOM_STSD:
				readStsd();
				break;
			case AtomType::ATOM_STTS:
				readStts();
				break;
			case AtomType::ATOM_STSC:
				readStsc();
				break;
			case AtomType::ATOM_STCO:
				readStco();
				break;
			case AtomType::ATOM_STSZ:
				readStsz();
				break;
			case AtomType::ATOM_SIDX:
				readSidx(size);
				break;
			case AtomType::ATOM_MOOF:
			case AtomType::ATOM_MDAT:
				// the track can be accessed randomly if all the tables are set before parsing the first fragment
				isSeekable = fragmentsLen || (chunksLen && chunkOffsetsLen && samplesLen && sampleSizesLen);
				if (type == AtomType::ATOM_MOOF) {
					// this will seek to the start of mdat header
					error = !parseMoof(size);
				} else {
					// pos already points to sample data
					isInData = true;
				}
				parsed = true;
				break;
			default:
				// ignore unknown atoms
				skipBytes(size);
				break;
		}
	}
	if (sampleDescLen) {
		codec = getCodec(sampleDesc);
	}
	if (!hasMoov || audioTrackId == -1 || codec == AudioCodec::UNKNOWN) {
		// this is not a progressive MP4, can't be played
		// or has no audio tracks
		// or has an unknown audio codec
		freeAll();
		isParsed = false;
		return false;
	}
	if (isInData) {
		// [pos] points to mdat, create a dummy fragment for it
		createFragment()->duration = totalDuration;
	}
	isParsed = !error && !closed;
	setCurrentFragment();
	setCurrentSample();
	return isParsed;
}

bool Mpeg4Container::parseMoof(uint32_t moofSize) {
	freeFragment();
	uint32_t size;
	AtomType type;
	uint32_t moofOffset = pos - 8;
	uint32_t moofEnd = pos + moofSize;
	uint32_t trafEnd = 0;

	bool hasFragment = false;
	Mpeg4Fragment *fragment;
	for (fragment = fragments; fragment < fragments + fragmentsLen; fragment++) {
		if (isInFragment(fragment, pos)) {
			hasFragment = true;
			break;
		}
	}

	while (pos < moofEnd) {
		readAtomHeader(size, (uint32_t &)type);
		switch (type) {
			case AtomType::ATOM_TRAF:
				trafEnd = pos + size;
				continue;
			case AtomType::ATOM_TFHD:
				readTfhd(trafEnd, moofOffset);
				break;
			case AtomType::ATOM_TRUN:
				readTrun(size, moofOffset);
				break;
			default:
				skipBytes(size);
				break;
		}
	}
	// this moof is not in the fragments table
	if (!hasFragment) {
		fragment = createFragment();
	}
	if (!totalDurationPresent) {
		// total duration was not found or a new fragment was created
		uint32_t duration = 0;
		for (Mpeg4SampleRange *sr = samples; sr < samples + samplesLen; sr++) {
			duration += sr->count * sr->duration;
		}
		fragment->duration = duration;
		totalDuration += duration;
		durationMs = totalDuration * 1000LL / timescale;
	}
	isFragmented = true;
	return true;
}

int32_t Mpeg4Container::getLoadingOffset(uint32_t timeMs) {
	if (!isParsed)
		return SAMPLE_NOT_LOADED;
	if (!isSeekable)
		return SAMPLE_NOT_SEEKABLE;
	// timeScaled - specified time in the media time coordinate system
	uint64_t timeScaled = (uint64_t)timeMs * timescale / 1000LL;
	uint64_t timeAbs = 0;

	Mpeg4Fragment *fragment = fragments;
	for (; fragment < fragments + fragmentsLen; fragment++) {
		timeAbs += fragment->duration; // timeAbs holds the fragment end time
		if (timeScaled < timeAbs) {
			timeAbs -= fragment->duration; // set timeAbs to fragment start time
			break;
		}
	}
	if (!fragment)
		return SAMPLE_NOT_FOUND;
	if (fragment != curFragment)
		return (int32_t)fragment->start;
	// get the position in bytes
	return (int32_t)findSample((int64_t)timeScaled, -1, timeAbs);
}

bool Mpeg4Container::goToData() {
	if (!isParsed || !curFragment)
		return false;
	if (isInData)
		return true;
	uint32_t size;
	AtomType type;

	if (pos == curFragment->start || pos >= curFragment->end) {
		// fragment ended, or a new one just loaded
		while (pos >= curFragment->end && curFragment < fragments + fragmentsLen - 1) {
			// skip to the next fragment header
			curFragment++;
		} // else, no more **loaded** fragments
		if (pos < curFragment->start && !skipTo(curFragment->start))
			return false;
		// [pos] is either a fragment header, EOF or unknown data
		readAtomHeader(size, (uint32_t &)type);
		if (type == AtomType::ATOM_MOOF) {
			// fragment header found, try to parse it
			parseMoof(size);
			// update [curFragment]
			setCurrentFragment();
			// read mdat header
			readAtomHeader(size, (uint32_t &)type);
		}
		if (type != AtomType::ATOM_MDAT)
			return false;
	} else if (pos >= curChunk.end) {
		// chunk ended, but still in [curFragment]
		if (!curChunk.nextStart) // no more chunks but fragment not ended ??
			return false;
		if (pos != curChunk.nextStart && !skipTo(curChunk.nextStart))
			return false;
	} /* else {
		 readAtomHeader(size, (uint32_t &)type);
		 return false;
	 }*/
	// update [isInData], [curChunk] and [curSampleSize]
	setCurrentSample();
	if (pos < curChunk.start) {
		// chunk not started yet, probably a multi-track movie
		if (!skipTo(curChunk.start))
			return false;
		// update [isInData], [curChunk] and [curSampleSize]
		setCurrentSample();
	}
	return true;
}

bool Mpeg4Container::seekTo(uint32_t timeMs) {
	if (!isParsed || !isSeekable)
		return false;
	// try to go to nearest mdat data
	if (!goToData())
		return false;
	uint32_t offset = getLoadingOffset(timeMs);
	// check if the required [offset] is in the currently loaded fragment
	//  - if it is, [offset] points to the required sample
	//  - if it isn't, [offset] points to a moof header
	if (!isInFragment(curFragment, offset)) {
		// try to seek to moof header, fail if not possible
		if (offset != pos && !skipTo(offset))
			return false;
		// [pos] points to a moof header
		isInData = false;
		// parse the just loaded atom header (pos >= curFragment->end)
		if (!goToData())
			return false;
		// get the actual sample's offset
		offset = getLoadingOffset(timeMs);
		// ...or give up if still not loaded
		if (!isInFragment(curFragment, offset))
			return false;
	}
	if (!isInData) // something is really not ok
		return false;
	// [pos] points to mdat data
	// [offset] points to the required sample
	if (!skipTo(offset))
		return false;
	// update the current chunk range and sample sizes
	setCurrentSample();
	return true;
}

int32_t Mpeg4Container::getCurrentTimeMs() {
	if (!curFragment || !isParsed)
		return 0;
	int64_t time = 0;
	// get time offset of the current fragment
	Mpeg4Fragment *f = fragments;
	while (f != curFragment) {
		time += (f++)->duration;
	}
	time = findSample(-1, (int32_t)pos, time);
	if (time < 0)
		return (int32_t)time;
	return (int32_t)(time * 1000LL / timescale);
}

uint8_t *Mpeg4Container::readSample(uint32_t &len) {
	if (!curFragment || !isParsed)
		return nullptr;
	if (!sampleData) {
		allocSampleData();
	}
	// go to mdat
	if (!isInData && !goToData())
		return nullptr;
	len = *curSampleSize;
	len = readBytes(sampleData, len);
	skipBytes(*curSampleSize - len); // skip the rest if something went wrong
	if (sampleSizesLen > 1)
		curSampleSize++;
	if (pos >= curChunk.end) {
		// chunk ended, make goToData() read the next one
		isInData = false;
	}
	return sampleData;
}

uint8_t *Mpeg4Container::getSetupData(uint32_t &len, AudioCodec matchCodec) {
	for (SampleDescription *desc = sampleDesc; desc < sampleDesc + sampleDescLen; desc++) {
		if (matchCodec != getCodec(desc))
			continue;
		len = desc->dataLength;
		return desc->data;
	}
	return nullptr;
}
