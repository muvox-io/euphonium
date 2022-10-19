#include "MpegDashDemuxer.h"

using namespace bell::mpeg;

MpegDashDemuxer::MpegDashDemuxer(std::shared_ptr<bell::ByteStream> stream)
{
    this->reader = std::make_shared<bell::BinaryReader>(stream);
}

int readIntFromVector(std::vector<uint8_t> c, size_t offset)
{

    return ((c[0] << 24) | (c[1] << 16) | (c[2] << 8) | (c[3])) & 0xffffffffL;
}

void MpegDashDemuxer::close() {
    reader->close();
}
void MpegDashDemuxer::parse()
{
    // Skip FTYP box
    lastBox = readBox();
    ensureBox(lastBox);

    auto moov = std::make_unique<Moov>();

    while (lastBox->type != ATOM_MOOF)
    {
        ensureBox(lastBox);
        lastBox = readBox();
        if (lastBox->type == ATOM_MOOV)
        {
            // parse moov
            moov = parseMoov(lastBox);
        }
    }

    tracks = std::vector<Mp4Track>(moov->trak.size());

    for (int i = 0; i < tracks.size(); i++)
    {
        tracks[i] = Mp4Track();
        tracks[i].trak = std::move(moov->trak[i]);
    }
}

int MpegDashDemuxer::parseMfhd()
{
    reader->skip(4);
    return reader->readInt();
}

long MpegDashDemuxer::parseTfdt()
{
    uint8_t version = reader->readByte();
    reader->skip(3);
    return version == 0 ? reader->readUInt() : reader->readLong();
}

std::unique_ptr<Mp4DashSample> MpegDashDemuxer::getNextSample(std::unique_ptr<Chunk> &chunk)
{
    auto sample = std::make_unique<Mp4DashSample>();
    if (chunk->size == 0)
    {
        return nullptr;
    }

    if (chunk->i >= chunk->moof->traf->trun->entryCount)
    {
        return nullptr;
    }

    sample->info = getAbsoluteTrunEntry(chunk->moof->traf->trun, chunk->i++, chunk->moof->traf->tfhd);
    sample->data = reader->readBytes(sample->info->sampleSize);
    chunk->sampleRead += sample->info->sampleSize;

    return sample;
}

std::unique_ptr<Chunk> MpegDashDemuxer::getNextChunk(bool infoOnly)
{

    while (reader->position() < reader->size())
    {
        if (chunkZero)
        {
            ensureBox(lastBox);
            lastBox = readBox();
        }
        else
        {
            chunkZero = true;
        }

        if (lastBox->type == ATOM_MOOF)
        {
            lastMoof = parseMoof(lastBox, tracks[0].trak->tkhd->trackId);

            if (lastMoof->traf != nullptr)
            {
                if (hasFlag(lastMoof->traf->trun->bFlags, 0x0001))
                {

                    lastMoof->traf->trun->dataOffset -= lastBox->size + 8;
                }
            }

            if (lastMoof->traf->trun->chunkSize < 1)
            {
                if (hasFlag(lastMoof->traf->tfhd->bFlags, 0x10))
                {
                    lastMoof->traf->trun->chunkSize = lastMoof->traf->tfhd->defaultSampleSize * lastMoof->traf->trun->entryCount;
                }
                else
                {
                    lastMoof->traf->trun->chunkSize = lastBox->size = 8;
                }
            }

            if (!hasFlag(lastMoof->traf->trun->bFlags, 0x900) && lastMoof->traf->trun->chunkDuration == 0)
            {
                if (hasFlag(lastMoof->traf->tfhd->bFlags, 0x20))
                {
                    lastMoof->traf->trun->chunkDuration = lastMoof->traf->tfhd->defaultSampleDuration * lastMoof->traf->trun->entryCount;
                }
            }
        }

        if (lastBox->type == ATOM_MDAT)
        {
            if (lastMoof->traf == nullptr)
            {
                lastMoof = nullptr;
                continue;
            }

            auto chunk = std::make_unique<Chunk>();
            chunk->moof = std::move(lastMoof);
            if (!infoOnly)
            {
                chunk->size = chunk->moof->traf->trun->chunkSize;
            }

            lastMoof = nullptr;
            reader->skip(chunk->moof->traf->trun->dataOffset);
            return chunk;
        }
    }

    return std::unique_ptr<Chunk>(nullptr);
}

size_t MpegDashDemuxer::position() {
    return reader->position();
}

std::unique_ptr<Moof> MpegDashDemuxer::parseMoof(std::unique_ptr<MpegBox> &ref, int trackId)
{
    auto moof = std::make_unique<Moof>();
    auto box = readBox();
    moof->mfgdSequenceNumber = parseMfhd();
    ensureBox(box);

    box = untilBox(ref, ATOM_TRAF);
    while (box)
    {
        moof->traf = parseTraf(box, trackId);
        ensureBox(box);

        if (moof->traf->tfdt != -1)
        {
            return moof;
        }
        box = untilBox(ref, ATOM_TRAF);
    }

    return moof;
}

std::unique_ptr<Traf> MpegDashDemuxer::parseTraf(std::unique_ptr<MpegBox> &ref, int trackId)
{
    auto traf = std::make_unique<Traf>();
    auto box = readBox();
    traf->tfhd = parseTfhd(trackId);
    ensureBox(box);

    box = untilBox(ref, ATOM_TRUN, ATOM_TFDT);

    if (box->type == ATOM_TFDT)
    {
        traf->tfdt = parseTfdt();
        ensureBox(box);
        box = readBox();
    }

    traf->trun = parseTrun();
    ensureBox(box);

    return traf;
}

std::unique_ptr<Trun> MpegDashDemuxer::parseTrun()
{
    auto trun = std::make_unique<Trun>();
    trun->bFlags = reader->readInt();
    trun->entryCount = reader->readInt();

    trun->entriesRowSize = 0;

    if (hasFlag(trun->bFlags, 0x0100))
    {
        trun->entriesRowSize += 4;
    }
    if (hasFlag(trun->bFlags, 0x0200))
    {
        trun->entriesRowSize += 4;
    }
    if (hasFlag(trun->bFlags, 0x0400))
    {
        trun->entriesRowSize += 4;
    }
    if (hasFlag(trun->bFlags, 0x0800))
    {
        trun->entriesRowSize += 4;
    }

    if (hasFlag(trun->bFlags, 0x0001))
    {
        trun->dataOffset = reader->readInt();
    }

    if (hasFlag(trun->bFlags, 0x0004))
    {
        trun->bFirstSampleFlags = reader->readInt();
    }

    trun->bEntries = reader->readBytes(trun->entriesRowSize * trun->entryCount);
    trun->chunkSize = 0;

    for (int i = 0; i < trun->entryCount; i++)
    {
        auto entry = getTrunEntry(trun, i);
        if (hasFlag(trun->bFlags, 0x0100))
        {
            trun->chunkDuration += entry->sampleDuration;
        }

        if (hasFlag(trun->bFlags, 0x200))
        {

            trun->chunkSize += entry->sampleSize;
        }

        if (hasFlag(trun->bFlags, 0x0800))
        {
            if (!hasFlag(trun->bFlags, 0x0100))
            {
                trun->chunkDuration += entry->sampleCompositionTimeOffset;
            }
        }
    }

    return trun;
}

std::unique_ptr<TrunEntry> MpegDashDemuxer::getTrunEntry(std::unique_ptr<Trun> &trun, int i)
{
    std::vector<uint8_t> subBuffer(
        trun->bEntries.begin() + (i * trun->entriesRowSize),
        trun->bEntries.begin() + ((i + 1) * trun->entriesRowSize));

    auto entry = std::make_unique<TrunEntry>();

    if (hasFlag(trun->bFlags, 0x0100))
    {
        entry->sampleDuration = readIntFromVector(subBuffer, 0);
    }

    if (hasFlag(trun->bFlags, 0x0200))
    {
        entry->sampleSize = readIntFromVector(subBuffer, 0);
    }

    if (hasFlag(trun->bFlags, 0x0400))
    {
        entry->sampleFlags = readIntFromVector(subBuffer, 0);
    }

    if (hasFlag(trun->bFlags, 0x800))
    {
        entry->sampleCompositionTimeOffset = readIntFromVector(subBuffer, 0);
    }

    entry->hasCompositionTimeOffset = hasFlag(trun->bFlags, 0x0800);
    entry->isKeyframe = !hasFlag(entry->sampleFlags, 0x10000);

    return entry;
}

std::unique_ptr<TrunEntry> MpegDashDemuxer::getAbsoluteTrunEntry(std::unique_ptr<Trun> &trun, int i, std::unique_ptr<Tfhd> &header)
{
    std::unique_ptr<TrunEntry> entry = getTrunEntry(trun, i);
    if (!hasFlag(trun->bFlags, 0x0100) && hasFlag(header->bFlags, 0x20))
    {
        entry->sampleFlags = header->defaultSampleFlags;
    }

    if (!hasFlag(trun->bFlags, 0x0200) && hasFlag(header->bFlags, 0x10))
    {
        entry->sampleSize = header->defaultSampleSize;
    }

    if (!hasFlag(trun->bFlags, 0x0100) && hasFlag(header->bFlags, 0x08))
    {
        entry->sampleDuration = header->defaultSampleDuration;
    }

    if (i == 0 && hasFlag(trun->bFlags, 0x0004))
    {
        entry->sampleFlags = trun->bFirstSampleFlags;
    }
    return entry;
}

std::unique_ptr<Tfhd> MpegDashDemuxer::parseTfhd(int trackId)
{
    auto tfhd = std::make_unique<Tfhd>();
    tfhd->bFlags = reader->readInt();
    tfhd->trackId = reader->readInt();

    if (trackId != -1 && tfhd->trackId != trackId)
    {
        return tfhd;
    }

    if (hasFlag(tfhd->bFlags, 0x01))
    {
        reader->skip(8);
    }

    if (hasFlag(tfhd->bFlags, 0x02))
    {
        reader->skip(4);
    }

    if (hasFlag(tfhd->bFlags, 0x08))
    {
        tfhd->defaultSampleDuration = reader->readInt();
    }

    if (hasFlag(tfhd->bFlags, 0x10))
    {
        tfhd->defaultSampleSize = reader->readInt();
    }

    if (hasFlag(tfhd->bFlags, 0x20))
    {
        tfhd->defaultSampleFlags = reader->readInt();
    }

    return tfhd;
}

bool MpegDashDemuxer::hasFlag(int flags, int mask)
{
    return (flags & mask) == mask;
}

std::unique_ptr<Moov> MpegDashDemuxer::parseMoov(std::unique_ptr<MpegBox> &ref)
{
    auto moov = std::make_unique<Moov>();

    auto box = readBox();

    moov->mvhd = parseMvhd();
    moov->mvexTrex = std::vector<Trex>();
    ensureBox(box);

    moov->trak = std::vector<std::unique_ptr<Trak>>();

    box = untilBox(ref, ATOM_TRAK, ATOM_MVEX);
    while (box)
    {

        if (box->type == ATOM_TRAK)
        {
            moov->trak.push_back(parseTrak(box));
        }

        if (box->type == ATOM_MVEX)
        {
            moov->mvexTrex = parseMvex(box, moov->mvhd->nextTrackId);
        }

        ensureBox(box);
        box = untilBox(ref, ATOM_TRAK, ATOM_MVEX);
    }

    return moov;
}

Trex MpegDashDemuxer::parseTrex()
{
    reader->skip(4);
    Trex trex;
    trex.trackId = reader->readInt();
    trex.defaultSampleDescriptionIndex = reader->readInt();
    trex.defaultSampleDuration = reader->readInt();
    trex.defaultSampleSize = reader->readInt();
    trex.defaultSampleFlags = reader->readInt();

    return trex;
}

std::vector<Trex> MpegDashDemuxer::parseMvex(std::unique_ptr<MpegBox> &ref, int possibleTrackCount)
{
    auto trexs = std::vector<Trex>();

    auto box = untilBox(ref, ATOM_TREX);
    while (box)
    {
        trexs.push_back(parseTrex());
        ensureBox(box);
        box = untilBox(ref, ATOM_TREX);
    }

    return trexs;
}

std::vector<uint8_t> MpegDashDemuxer::readFullBox(std::unique_ptr<MpegBox> &ref)
{
    auto size = ref->size;
    std::vector<uint8_t> header(8);
    header.at(0) = *((uint8_t *)&ref->size + 0);
    header.at(1) = *((uint8_t *)&ref->size + 1);
    header.at(2) = *((uint8_t *)&ref->size + 2);
    header.at(3) = *((uint8_t *)&ref->size + 3);
    header.at(4) = *((uint8_t *)&ref->type + 0);
    header.at(5) = *((uint8_t *)&ref->type + 1);
    header.at(6) = *((uint8_t *)&ref->type + 2);
    header.at(7) = *((uint8_t *)&ref->type + 3);

    std::vector<uint8_t> data = reader->readBytes(size - 8);

    header.insert(header.end(), data.begin(), data.end());

    return header;
}

std::unique_ptr<Tkhd> MpegDashDemuxer::parseTkhd()
{
    auto tkhd = std::make_unique<Tkhd>();
    uint8_t version = reader->readByte();

    // flags
    // creation entries_time
    // modification entries_time
    reader->skip(3 + (2 * (version == 0 ? 4 : 8)));
    tkhd->trackId = reader->readInt();
    reader->skip(4);
    tkhd->duration = version == 0 ? reader->readUInt() : reader->readLong();
    reader->skip(2 * 4);
    tkhd->bLayer = reader->readShort();
    tkhd->bAlternateGroup = reader->readShort();
    tkhd->bVolume = reader->readShort();

    reader->skip(2);
    tkhd->matrix = reader->readBytes(9 * 4);
    tkhd->bWidth = reader->readInt();
    tkhd->bHeight = reader->readInt();

    return tkhd;
}

std::unique_ptr<Trak> MpegDashDemuxer::parseTrak(std::unique_ptr<MpegBox> &ref)
{
    auto trak = std::make_unique<Trak>();
    auto box = readBox();
    trak->tkhd = parseTkhd();
    ensureBox(box);

    box = untilBox(ref, ATOM_MDIA, ATOM_EDTS);
    while (box)
    {
        if (box->type == ATOM_MDIA)
        {
            trak->mdia = parseMdia(box);
        }

        if (box->type == ATOM_EDTS)
        {
            trak->edstElst = parseEdts(box);
        }

        ensureBox(box);
        box = untilBox(ref, ATOM_MDIA, ATOM_EDTS);
    }

    return trak;
}

std::unique_ptr<Minf> MpegDashDemuxer::parseMinf(std::unique_ptr<MpegBox> &ref)
{
    auto minf = std::make_unique<Minf>();
    auto box = untilAnyBox(ref);
    while (box)
    {
        if (box->type == ATOM_DINF)
        {
            minf->dinf = readFullBox(box);
        }

        if (box->type == ATOM_STBL)
        {
            minf->stblStsd = parseStbl(box);
        }
        if (box->type == ATOM_VMHD || box->type == ATOM_SMHD)
        {
            minf->mhd = readFullBox(box);
        }

        ensureBox(box);
        box = untilAnyBox(ref);
    }

    return minf;
}

std::vector<uint8_t> MpegDashDemuxer::parseStbl(std::unique_ptr<MpegBox> &ref)
{
    auto box = untilBox(ref, ATOM_STSD);

    return readFullBox(box);
}

std::unique_ptr<Elst> MpegDashDemuxer::parseEdts(std::unique_ptr<MpegBox> &ref)
{
    auto elst = std::make_unique<Elst>();
    auto box = untilBox(ref, ATOM_ELST);

    bool v1 = reader->readByte() == 1;
    reader->skip(3); // flags

    int entryCount = reader->readInt();
    if (entryCount < 1)
    {
        elst->bMediaRate = 0x00010000;
        return elst;
    }

    if (v1)
    {
        reader->skip(8); // duration
        elst->mediaTime = reader->readLong();
        // ignore all entries
        reader->skip((entryCount - 1) * 16);
    }
    else
    {
        reader->skip(4); // segment duration
        elst->mediaTime = reader->readInt();
    }

    elst->bMediaRate = reader->readInt();
    return elst;
}

std::unique_ptr<Hdlr> MpegDashDemuxer::parseHdlr(std::unique_ptr<MpegBox> &box)
{
    auto hdlr = std::make_unique<Hdlr>();
    reader->skip(4);

    hdlr->type = reader->readInt();
    hdlr->subType = reader->readInt();
    hdlr->bReserved = reader->readBytes(12);
    reader->skip((box->offset + box->size) - reader->position());
    return hdlr;
}

std::unique_ptr<Mdia> MpegDashDemuxer::parseMdia(std::unique_ptr<MpegBox> &ref)
{
    auto mdia = std::make_unique<Mdia>();

    auto box = untilBox(ref, ATOM_MDHD, ATOM_HDLR, ATOM_MINF);
    while (box)
    {
        if (box->type == ATOM_MDHD)
        {
            mdia->mdhd = readFullBox(box);
        }

        if (box->type == ATOM_HDLR)
        {
            mdia->hdlr = parseHdlr(box);
        }

        if (box->type == ATOM_MINF)
        {
            mdia->minf = parseMinf(box);
        }

        ensureBox(box);
        box = untilBox(ref, ATOM_MDHD, ATOM_HDLR, ATOM_MINF);
    }

    return mdia;
}

std::unique_ptr<Mvhd> MpegDashDemuxer::parseMvhd()
{
    auto mvhd = std::make_unique<Mvhd>();
    uint8_t version = reader->readByte();
    reader->skip(3); // flags

    // creation entries_time
    // modification entries_time
    reader->skip(2 * (version == 0 ? 4 : 8));

    mvhd->timeScale = reader->readUInt();

    // chunkDuration
    reader->skip(version == 0 ? 4 : 8);

    // rate
    // volume
    // reserved
    // matrix array
    // predefined
    reader->skip(76);

    mvhd->nextTrackId = reader->readUInt();

    return mvhd;
}

void MpegDashDemuxer::ensureBox(std::unique_ptr<MpegBox> &box)
{
    reader->skip(box->offset + box->size - reader->position());
}

std::unique_ptr<MpegBox> MpegDashDemuxer::readBox()
{
    auto box = std::make_unique<MpegBox>();
    box->offset = reader->position();
    box->size = reader->readUInt();
    box->type = reader->readInt();
    if (box->size == 1)
    {
        box->size = reader->readLong();
    }

    return box;
}

std::unique_ptr<MpegBox> MpegDashDemuxer::untilBox(std::unique_ptr<MpegBox> &ref, int boxType1, int boxType2, int boxType3)
{
    auto box = std::make_unique<MpegBox>();
    while (reader->position() < (ref->offset + ref->size))
    {
        box = readBox();
        if (box->type == boxType1 || box->type == boxType2 || box->type == boxType3)
        {
            return box;
        }
        ensureBox(box);
    }

    return std::unique_ptr<MpegBox>(nullptr);
}

std::unique_ptr<MpegBox> MpegDashDemuxer::untilBox(std::unique_ptr<MpegBox> &ref, int boxType1, int boxType2)
{
    auto box = std::make_unique<MpegBox>();
    while (reader->position() < (ref->offset + ref->size))
    {
        box = readBox();
        if (box->type == boxType1 || box->type == boxType2)
        {
            return box;
        }
        ensureBox(box);
    }

    return std::unique_ptr<MpegBox>(nullptr);
}

std::unique_ptr<MpegBox> MpegDashDemuxer::untilBox(std::unique_ptr<MpegBox> &ref, int boxType1)
{
    auto box = std::make_unique<MpegBox>();

    while (reader->position() < (ref->offset + ref->size))
    {
        box = readBox();
        if (box->type == boxType1)
        {
            return box;
        }
        ensureBox(box);
    }

    return std::unique_ptr<MpegBox>(nullptr);
}

std::unique_ptr<MpegBox> MpegDashDemuxer::untilAnyBox(std::unique_ptr<MpegBox> &ref)
{
    if (reader->position() >= (ref->offset + ref->size))
    {
        return std::unique_ptr<MpegBox>(nullptr);
    }

    return readBox();
}