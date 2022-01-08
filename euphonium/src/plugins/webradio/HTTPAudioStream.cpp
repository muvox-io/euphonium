#include "HTTPAudioStream.h"

HTTPAudioStream::HTTPAudioStream()
{
    bell::decodersInstance->ensureAAC();
    bell::decodersInstance->ensureMP3();
    inputBuffer = std::vector<uint8_t>(AAC_READBUF_SIZE * 2);
    outputBuffer = std::vector<short>(AAC_MAX_NCHANS * AAC_MAX_NSAMPS * 2 * 4);
    decodePtr = inputBuffer.data();
}

HTTPAudioStream::~HTTPAudioStream()
{
}

void HTTPAudioStream::querySongFromUrl(std::string url, AudioCodec audioCodec)
{
    codec = audioCodec;
    httpStream = std::make_shared<bell::HTTPStream>();
    httpStream->connectToUrl(url);
    decodePtr = inputBuffer.data();
    bytesLeft = 0;
    offset = 0;
}

void HTTPAudioStream::decodeFrameMP3(std::shared_ptr<MainAudioBuffer> audioBuffer)
{
    auto bufSize = MP3_READBUF_SIZE;
    bufSize = bufSize * 2;

    int readBytes = httpStream->read(inputBuffer.data() + bytesLeft, bufSize - bytesLeft);
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

void HTTPAudioStream::decodeFrameAAC(std::shared_ptr<MainAudioBuffer> audioBuffer)
{
    auto bufSize = AAC_READBUF_SIZE;

    int readBytes = httpStream->read(inputBuffer.data() + bytesLeft, bufSize - bytesLeft);
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

void HTTPAudioStream::decodeFrame(std::shared_ptr<MainAudioBuffer> audioBuffer)
{
    if (codec == AudioCodec::MP3)
    {
        decodeFrameMP3(audioBuffer);
    }
    else
    {
        decodeFrameAAC(audioBuffer);
    }
}
