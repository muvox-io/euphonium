#ifndef BELL_MPEG_DASH_TYPES_H
#define BELL_MPEG_DASH_TYPES_H

#include <vector>
#include <memory>

namespace bell::mpeg
{

    struct Tkhd
    {
        int trackId;
        long duration;
        short bVolume;
        int bWidth;
        int bHeight;
        std::vector<uint8_t> matrix;
        short bLayer;
        short bAlternateGroup;
    };

    struct Elst
    {
        long mediaTime;
        int bMediaRate;
    };

    struct Trex
    {
        int trackId;
        int defaultSampleDescriptionIndex;
        int defaultSampleDuration;
        int defaultSampleSize;
        int defaultSampleFlags;
    };

    struct Mvhd
    {
        long timeScale;
        long nextTrackId;
    };

    struct Hdlr
    {
        int type;
        int subType;
        std::vector<uint8_t> bReserved;
    };

    struct Minf
    {
        std::vector<uint8_t> dinf;
        std::vector<uint8_t> stblStsd;
        std::vector<uint8_t> mhd;
    };

    struct Tfhd
    {
        int bFlags;
        int trackId;
        int defaultSampleDuration;
        int defaultSampleSize;
        int defaultSampleFlags;
    };

    struct TrunEntry
    {
        int sampleDuration;
        int sampleSize;
        int sampleFlags;
        int sampleCompositionTimeOffset;
        bool hasCompositionTimeOffset;
        bool isKeyframe;
    };

    struct Trun
    {
        int chunkDuration;
        int chunkSize;
        int bFlags;
        int bFirstSampleFlags;
        int dataOffset;
        int entryCount;
        std::vector<uint8_t> bEntries;
        int entriesRowSize;
    };

    struct Traf
    {
        std::unique_ptr<Tfhd> tfhd;
        long tfdt = -1;
        std::unique_ptr<Trun> trun;
    };

    struct Moof
    {
        int mfgdSequenceNumber;
        std::unique_ptr<Traf> traf;
    };
    struct Chunk
    {
        std::vector<uint8_t> data = std::vector<uint8_t>();
        std::unique_ptr<Moof> moof;
        int i = 0;
        int size = 0;
        int sampleRead = 0;
    };

    struct Mp4DashSample
    {
        std::unique_ptr<TrunEntry> info;
        std::vector<uint8_t> data = std::vector<uint8_t>();
    };

    struct Mdia
    {
        int mhdTimeScale;
        std::vector<uint8_t> mdhd;
        std::unique_ptr<Hdlr> hdlr;
        std::unique_ptr<Minf> minf;
    };

    struct Trak
    {
        std::unique_ptr<Tkhd> tkhd;
        std::unique_ptr<Elst> edstElst;
        std::unique_ptr<Mdia> mdia;
    };

    struct Mp4Track
    {
        std::unique_ptr<Trak> trak;
    };

    struct Moov
    {
        std::unique_ptr<Mvhd> mvhd;
        std::vector<std::unique_ptr<Trak>> trak;
        std::vector<Trex> mvexTrex;
    };

    struct MpegBox
    {
        long size = 0;
        int32_t type = 0;
        long offset;
    };

}

#define ATOM_MOOF 0x6D6F6F66
#define ATOM_MFHD 0x6D666864
#define ATOM_TRAF 0x74726166
#define ATOM_TFHD 0x74666864
#define ATOM_TFDT 0x74666474
#define ATOM_TRUN 0x7472756E
#define ATOM_MDIA 0x6D646961
#define ATOM_FTYP 0x66747970
#define ATOM_SIDX 0x73696478
#define ATOM_MOOV 0x6D6F6F76
#define ATOM_MDAT 0x6D646174
#define ATOM_MVHD 0x6D766864
#define ATOM_TRAK 0x7472616B
#define ATOM_MVEX 0x6D766578
#define ATOM_TREX 0x74726578
#define ATOM_TKHD 0x746B6864
#define ATOM_MFRA 0x6D667261
#define ATOM_MDHD 0x6D646864
#define ATOM_EDTS 0x65647473
#define ATOM_ELST 0x656C7374
#define ATOM_HDLR 0x68646C72
#define ATOM_MINF 0x6D696E66
#define ATOM_DINF 0x64696E66
#define ATOM_STBL 0x7374626C
#define ATOM_STSD 0x73747364
#define ATOM_VMHD 0x766D6864
#define ATOM_SMHD 0x736D6864

#endif