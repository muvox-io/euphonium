#include "BinaryReader.h"
#include "ByteStream.h"
#include "MpegDashTypes.h"
#include <memory>

namespace bell::mpeg
{
    class MpegDashDemuxer
    {
        std::shared_ptr<bell::BinaryReader> reader;
        std::unique_ptr<bell::mpeg::MpegBox> lastBox;
        std::unique_ptr<bell::mpeg::Moof> lastMoof;
        std::vector<bell::mpeg::Mp4Track> tracks;
        bool chunkZero = false;

    public:
        MpegDashDemuxer(std::shared_ptr<ByteStream> stream);
        ~MpegDashDemuxer() {
            reader->close();
        }
        void parse();
        std::unique_ptr<bell::mpeg::MpegBox> readBox();
        // std::vector<Mp4Track> getTracks();
        std::vector<uint8_t> readFullBox(std::unique_ptr<bell::mpeg::MpegBox> &);
        void ensureBox(std::unique_ptr<bell::mpeg::MpegBox> &);
        size_t position();
        void close();
        std::unique_ptr<bell::mpeg::Mvhd> parseMvhd();
        std::unique_ptr<bell::mpeg::Chunk> getNextChunk(bool infoOnly);
        std::unique_ptr<bell::mpeg::TrunEntry> getTrunEntry(std::unique_ptr<Trun> &, int);
        std::unique_ptr<bell::mpeg::TrunEntry> getAbsoluteTrunEntry(std::unique_ptr<bell::mpeg::Trun> &, int, std::unique_ptr<bell::mpeg::Tfhd> &);
        std::vector<bell::mpeg::Trex> parseMvex(std::unique_ptr<bell::mpeg::MpegBox> &, int possibleTrackCount);
        std::unique_ptr<bell::mpeg::Mp4DashSample> getNextSample(std::unique_ptr<bell::mpeg::Chunk> &);
        bool hasFlag(int flags, int mask);
        std::unique_ptr<bell::mpeg::Hdlr> parseHdlr(std::unique_ptr<bell::mpeg::MpegBox> &);
        std::unique_ptr<bell::mpeg::Moov> parseMoov(std::unique_ptr<bell::mpeg::MpegBox> &);
        int parseMfhd();
        std::unique_ptr<bell::mpeg::Traf> parseTraf(std::unique_ptr<bell::mpeg::MpegBox> &, int);
        std::unique_ptr<bell::mpeg::Moof> parseMoof(std::unique_ptr<bell::mpeg::MpegBox> &, int);
        std::unique_ptr<bell::mpeg::Tfhd> parseTfhd(int);
        long parseTfdt();
        std::unique_ptr<bell::mpeg::Trun> parseTrun();
        std::unique_ptr<bell::mpeg::Tkhd> parseTkhd();
        std::unique_ptr<bell::mpeg::Trak> parseTrak(std::unique_ptr<bell::mpeg::MpegBox> &);
        bell::mpeg::Trex parseTrex();
        std::unique_ptr<bell::mpeg::Mdia> parseMdia(std::unique_ptr<bell::mpeg::MpegBox> &);
        std::unique_ptr<bell::mpeg::Minf> parseMinf(std::unique_ptr<bell::mpeg::MpegBox> &);
        std::unique_ptr<bell::mpeg::Elst> parseEdts(std::unique_ptr<bell::mpeg::MpegBox> &);
        std::vector<uint8_t> parseStbl(std::unique_ptr<bell::mpeg::MpegBox> &);
        std::unique_ptr<bell::mpeg::MpegBox> untilBox(std::unique_ptr<bell::mpeg::MpegBox> &, int, int, int);
        std::unique_ptr<bell::mpeg::MpegBox> untilBox(std::unique_ptr<bell::mpeg::MpegBox> &, int, int);
        std::unique_ptr<bell::mpeg::MpegBox> untilBox(std::unique_ptr<bell::mpeg::MpegBox> &, int);
        std::unique_ptr<bell::mpeg::MpegBox> untilAnyBox(std::unique_ptr<MpegBox> &);
    };
}