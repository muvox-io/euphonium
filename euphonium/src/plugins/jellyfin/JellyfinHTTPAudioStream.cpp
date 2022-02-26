#include "JellyfinHTTPAudioStream.h"

JellyfinHTTPAudioStream::JellyfinHTTPAudioStream()
{
    bell::decodersInstance->ensureAAC();
    bell::decodersInstance->ensureMP3();
    inputBuffer = std::vector<uint8_t>(AAC_READBUF_SIZE * 2);
    outputBuffer = std::vector<short>(AAC_MAX_NCHANS * AAC_MAX_NSAMPS * 2 * 4);
    decodePtr = inputBuffer.data();
}

JellyfinHTTPAudioStream::~JellyfinHTTPAudioStream()
{
}

void JellyfinHTTPAudioStream::querySongFromUrl(std::string url, JellyfinAudioCodec audioCodec, std::map<std::string, std::string> headers)
{
    codec = audioCodec;
    client = std::make_shared<bell::HTTPClient>();

    headers.insert({std::string("Connection"), std::string("keep-alive")});
    
    struct bell::HTTPClient::HTTPRequest request = {
		.method = bell::HTTPClient::HTTPMethod::GET,
		.url = url,
		.body = nullptr,
		.contentType = nullptr,
		.headers = headers,
		.maxRedirects = -1,
		.dumpFs = nullptr,
		.dumpRawFs = nullptr,
    };
    
    response = client->execute(request);

    decodePtr = inputBuffer.data();
    bytesLeft = 0;
    offset = 0;
}

void JellyfinHTTPAudioStream::decodeFrameMP3(std::shared_ptr<MainAudioBuffer> audioBuffer)
{
    auto bufSize = MP3_READBUF_SIZE;
    bufSize = bufSize * 2;

    int readBytes = response->read(inputBuffer.data() + bytesLeft, bufSize - bytesLeft);
    if (readBytes > 0)
    {
        bytesLeft = readBytes + bytesLeft;
        decodePtr = inputBuffer.data();
        offset = MP3FindSyncWord(inputBuffer.data(), bytesLeft);

        if (offset != -1)
        {
            bytesLeft -= offset;
            decodePtr += offset;

            int decodeStatus = MP3Decode(bell::decodersInstance->mp3Decoder, &decodePtr, &bytesLeft, outputBuffer.data(), 0);
            MP3GetLastFrameInfo(bell::decodersInstance->mp3Decoder, &mp3FrameInfo);
            if (decodeStatus == ERR_MP3_NONE)
            {
                const uint8_t *audioData = reinterpret_cast<const uint8_t *>(outputBuffer.data());
                currentSampleRate = mp3FrameInfo.samprate;
                auto sizeData = (mp3FrameInfo.bitsPerSample / 8) * mp3FrameInfo.outputSamps;

                size_t bytesWritten = 0;
                while (bytesWritten < sizeData)
                {
                    bytesWritten += audioBuffer->write(audioData + bytesWritten, sizeData - bytesWritten);
                }
            }
            else
            {
                EUPH_LOG(info, "webradio", "Error in frame, moving two bytes %d", decodeStatus);
                decodePtr += 1;
                bytesLeft -= 1;
            }
        }
        else
        {
            EUPH_LOG(info, "webradio", "Big error");
            decodePtr += 3800;
            bytesLeft -= 3800;
        }

        memmove(inputBuffer.data(), decodePtr, bytesLeft);
    }
}

void JellyfinHTTPAudioStream::decodeFrameAAC(std::shared_ptr<MainAudioBuffer> audioBuffer)
{
    auto bufSize = AAC_READBUF_SIZE;

    int readBytes = response->read(inputBuffer.data() + bytesLeft, bufSize - bytesLeft);
    if (readBytes > 0)
    {
        bytesLeft = readBytes + bytesLeft;
        decodePtr = inputBuffer.data();
        offset = AACFindSyncWord(inputBuffer.data(), bytesLeft);

        if (offset != -1)
        {
            bytesLeft -= offset;
            decodePtr += offset;

            int decodeStatus = AACDecode(bell::decodersInstance->aacDecoder, &decodePtr, &bytesLeft, outputBuffer.data());
            AACGetLastFrameInfo(bell::decodersInstance->aacDecoder, &aacFrameInfo);
            if (decodeStatus == ERR_AAC_NONE)
            {
                const uint8_t *audioData = reinterpret_cast<const uint8_t *>(outputBuffer.data());
                currentSampleRate = aacFrameInfo.sampRateOut;
                auto sizeData = (aacFrameInfo.bitsPerSample / 8) * aacFrameInfo.outputSamps;

                size_t bytesWritten = 0;
                while (bytesWritten < sizeData)
                {
                    bytesWritten += audioBuffer->write(audioData + bytesWritten, sizeData - bytesWritten);
                }
            }
            else
            {
                EUPH_LOG(info, "webradio", "Error in frame, moving two bytes %d", decodeStatus);
                decodePtr += 1;
                bytesLeft -= 1;
            }
        }
        else
        {
            EUPH_LOG(info, "webradio", "Big error");
            decodePtr += 3800;
            bytesLeft -= 3800;
        }

        memmove(inputBuffer.data(), decodePtr, bytesLeft);
    }
}

void JellyfinHTTPAudioStream::decodeFrame(std::shared_ptr<MainAudioBuffer> audioBuffer)
{
    if (codec == JellyfinAudioCodec::MP3)
    {
        decodeFrameMP3(audioBuffer);
    }
    else
    {
        decodeFrameAAC(audioBuffer);
    }
}
