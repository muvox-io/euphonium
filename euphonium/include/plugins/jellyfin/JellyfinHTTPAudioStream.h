#ifndef EUPHONIUM_JELLYFIN_HTTP_AUDIO_STREAM_H
#define EUPHONIUM_JELLYFIN_HTTP_AUDIO_STREAM_H

#include <string>
#include <memory>
#include <map>
#include "HTTPClient.h"
#include "aacdec.h"
#include "mp3dec.h"
#include "MainAudioBuffer.h"
#include "DecoderGlobals.h"
#include "BellUtils.h"
#include <vector>

enum class JellyfinAudioCodec {
    AAC,
    MP3
};

class JellyfinHTTPAudioStream {
    private:

    AACFrameInfo aacFrameInfo;
    MP3FrameInfo mp3FrameInfo;
    uint8_t *decodePtr;

    // std::shared_ptr<bell::HTTPStream> httpStream;
    std::shared_ptr<bell::HTTPClient> client;
    bell::HTTPClient::HTTPResponse_t response;
    std::vector<uint8_t> inputBuffer;
    std::vector<short> outputBuffer;
    JellyfinAudioCodec codec = JellyfinAudioCodec::AAC;
    int bytesLeft = 0;
    int offset = 0;
    void decodeFrameMP3(std::shared_ptr<MainAudioBuffer> circularBuffer);
    void decodeFrameAAC(std::shared_ptr<MainAudioBuffer> circularBuffer);

    public:
    JellyfinHTTPAudioStream();
    ~JellyfinHTTPAudioStream();
    uint32_t currentSampleRate = 0;

    void querySongFromUrl(std::string url, JellyfinAudioCodec audioCodec, std::map<std::string, std::string> headers = {});
    void decodeFrame(std::shared_ptr<MainAudioBuffer> circularBuffer);
};
#endif
