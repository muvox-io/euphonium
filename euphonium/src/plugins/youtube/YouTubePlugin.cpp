#include "YouTubePlugin.h"
#include <HTTPStream.h>
#include <thread>

std::vector<uint8_t> getAdtsHeader(int length)
{
    int profile = 2;
    int chanCfg = 2;

    // 0: 96000 Hz
    // 1: 88200 Hz
    // 2: 64000 Hz
    // 3: 48000 Hz
    // 4: 44100 Hz
    // 5: 32000 Hz
    int freqIdx = 4;

    auto res = std::vector<uint8_t>(7);
    res[0] = 0xFF;
    res[1] = 0xF9;
    res[2] = (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
    res[3] = (((chanCfg & 3) << 6) + (length >> 11));
    res[4] = ((length & 0x7FF) >> 3);
    res[5] = (((length & 7) << 5) + 0x1F);
    res[6] = 0xFC;
    return res;
}

YouTubePlugin::YouTubePlugin() : bell::Task("youtube", 6 * 1024, 1, false)
{
    name = "youtube";
    outputBuffer = std::vector<short>(AAC_MAX_NCHANS * AAC_MAX_NSAMPS * 2 * 4);

    bell::decodersInstance->ensureAAC();
}

void YouTubePlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader)
{
    scriptLoader->loadScript("youtube_plugin", luaState);
}

void YouTubePlugin::setupLuaBindings()
{
    sol::state_view lua(luaState);
    lua.set_function("youtubeQueueUrl", &YouTubePlugin::playYTUrl, this);
}

void YouTubePlugin::configurationUpdated()
{
}

void YouTubePlugin::playYTUrl(std::string url)
{
    isRunning = false;
    ytUrlQueue.push(url);
}

void YouTubePlugin::shutdown()
{
    EUPH_LOG(info, "youtube", "Shutting down...");
    isRunning = false;
    std::lock_guard lock(runningMutex);
    status = ModuleStatus::SHUTDOWN;
}

void YouTubePlugin::runTask()
{
    std::string url;

    while (true)
    {
        if (this->ytUrlQueue.wpop(url))
        {
            std::lock_guard lock(runningMutex);
            isRunning = true;
            status = ModuleStatus::RUNNING;

            auto ytHandler = std::make_shared<bell::HTTPStream>();
            ytHandler->connectToUrl(url);

            auto demuxer = std::make_shared<bell::mpeg::MpegDashDemuxer>(ytHandler);
            demuxer->parse();

            auto currentChunk = demuxer->getNextChunk(false);

            while (currentChunk && isRunning)
            {
                EUPH_LOG(info, "yt", "Received chunk with size %d", currentChunk->size);
                auto sample = demuxer->getNextSample(currentChunk);

                while (sample && isRunning)
                {
                    auto adtsHeader = getAdtsHeader(sample->data.size() + 7);
                    sample->data.insert(sample->data.begin(), adtsHeader.begin(), adtsHeader.end());

                    int size = sample->data.size();
                    unsigned char *data = reinterpret_cast<unsigned char *>(sample->data.data());

                    int decodeStatus = AACDecode(bell::decodersInstance->aacDecoder, &data, &size, outputBuffer.data());
                    AACGetLastFrameInfo(bell::decodersInstance->aacDecoder, &aacFrameInfo);
                    if (decodeStatus == ERR_AAC_NONE)
                    {
                        const uint8_t *audioData = reinterpret_cast<const uint8_t *>(outputBuffer.data());
                        auto sizeData = (aacFrameInfo.outputSamps / aacFrameInfo.nChans) * sizeof(uint32_t);

                        size_t bytesWritten = 0;
                        while (bytesWritten < sizeData)
                        {
                            bytesWritten += audioBuffer->write(audioData + bytesWritten, sizeData - bytesWritten);
                        }
                    }
                    sample = demuxer->getNextSample(currentChunk);
                }

                currentChunk = demuxer->getNextChunk(false);
            }
            demuxer->close();
        }
    }
}

void YouTubePlugin::startAudioThread()
{
    startTask();
}