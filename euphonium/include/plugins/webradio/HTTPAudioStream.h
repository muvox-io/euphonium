#ifndef EUPHONIUM_HTTP_AUDIO_STREAM_H
#define EUPHONIUM_HTTP_AUDIO_STREAM_H

#include <string>
#include <memory>
#include "HTTPStream.h"
#include "aacdec.h"
#include "mp3dec.h"
#include "MainAudioBuffer.h"
#include "BellUtils.h"
#include <vector>

#define AAC_READBUF_SIZE (4 * AAC_MAINBUF_SIZE * AAC_MAX_NCHANS)
#define MP3_READBUF_SIZE (2 * 1024);

enum class AudioCodec {
    AAC,
    MP3
};

class HTTPAudioStream {
    private:
    HAACDecoder decoder;
    HMP3Decoder mp3Decoder;
    AACFrameInfo aacFrameInfo;
    MP3FrameInfo mp3FrameInfo;
    uint8_t *decodePtr;

    std::shared_ptr<HTTPStream> httpStream;
    std::vector<uint8_t> inputBuffer;
    std::vector<short> outputBuffer;
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