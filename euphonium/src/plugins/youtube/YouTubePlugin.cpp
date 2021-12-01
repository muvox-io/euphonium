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

YouTubePlugin::YouTubePlugin() : bell::Task("youtube", 16 * 1024, 1)
{
    name = "youtube";
    outputBuffer = std::vector<short>(AAC_MAX_NCHANS * AAC_MAX_NSAMPS * 2 * 4);

    bell::decodersInstance->ensureAAC();
}

void YouTubePlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader)
{
    scriptLoader->loadScript("youtube_plugin", berry);
}

void YouTubePlugin::setupBindings()
{
    berry->export_this("youtubeQueueUrl", this, &YouTubePlugin::playYTUrl);
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

std::string YouTubePlugin::getStreamForVideo(std::string videoUrl)
{
    auto socket = std::make_unique<bell::TLSSocket>();
    socket->open("https://youtubei.googleapis.com/youtubei/v1/player?key=" + std::string(YT_PLAYER_KEY));

    auto body = std::string(POST_BODY_PART1) + videoUrl + std::string(POST_BODY_PART2) + videoUrl + std::string(POST_BODY_PART3);
    std::stringstream ss;
    ss << "POST /youtubei/v1/player?key=" << YT_PLAYER_KEY << " HTTP/1.1\r\n"
       << "Host: youtubei.googleapis.com:443\r\n"
       << "Accept: */*\r\n"
       << "Content-Type: application/json\r\n"
       << "Content-Length: " << body.length() << "\r\n"
       << "\r\n"
       << body;
    auto req = ss.str();

    socket->write((uint8_t *)req.c_str(), req.length());

    std::vector<uint8_t> buffer(128);

    size_t nbytes = 0;
    std::string currentLine = "";
    bool gotItag = false;
    bool isFinished = false;

    while (nbytes >= 0 && !isFinished)
    {
        // Read line by line from socket
        nbytes = socket->read(&buffer[0], buffer.size());

        currentLine += std::string(buffer.data(), buffer.data() + nbytes);
        while (currentLine.find("\n") != std::string::npos)
        {
            auto line = currentLine.substr(0, currentLine.find("\n"));
            currentLine = currentLine.substr(currentLine.find("\n") + 1, currentLine.size());

            if (gotItag && line.find("\"url\"") != std::string::npos)
            {

                EUPH_LOG(info, "youtube", "Got url: %s", line.c_str());
                // get substring after ""url": ""
                auto url = line.substr(line.find("\"url\": \"") + 8);
                url = url.substr(0, url.find("\""));
                EUPH_LOG(info, "youtube", "Got url: %s", url.c_str());
                isFinished = true;

                return url;
            }

            if (line.find("\"itag\": 140") != std::string::npos)
            {
                gotItag = true;
                BELL_LOG(info, "youtube", "Got itag: 140");
            }
        }
    }

    return "";
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
            audioBuffer->shutdownExcept(name);
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
