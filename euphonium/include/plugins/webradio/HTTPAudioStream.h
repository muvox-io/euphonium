#ifndef EUPHONIUM_HTTP_AUDIO_STREAM_H
#define EUPHONIUM_HTTP_AUDIO_STREAM_H

#include <string>
#include <memory>
#include "HTTPStream.h"
#include "aacdec.h"
#include "mp3dec.h"
#include "MainAudioBuffer.h"
#include "DecoderGlobals.h"
#include "BellUtils.h"
#include <vector>

enum class AudioCodec {
    AAC,
    MP3
};

class HTTPAudioStream {
    private:

    AACFrameInfo aacFrameInfo;
    MP3FrameInfo mp3FrameInfo;
    uint8_t *decodePtr;

    std::shared_ptr<bell::HTTPStream> httpStream;
    std::vector<uint8_t> inputBuffer;
    std::vector<short> outputBuffer;
    int mutedSamples = 0;
    AudioCodec codec = AudioCodec::AAC;
    int bytesLeft = 0;
    int offset = 0;
    void decodeFrameMP3(std::shared_ptr<MainAudioBuffer> circularBuffer);
    void decodeFrameAAC(std::shared_ptr<MainAudioBuffer> circularBuffer);

    public:
    HTTPAudioStream();
    ~HTTPAudioStream();
    void querySongFromUrl(std::string url, AudioCodec audioCodec);
    void decodeFrame(std::shared_ptr<MainAudioBuffer> circularBuffer);
};
#endif
