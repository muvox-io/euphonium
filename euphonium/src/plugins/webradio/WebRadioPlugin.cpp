#include "WebRadioPlugin.h"
#include <HTTPStream.h>
#include <thread>

WebRadioPlugin::WebRadioPlugin(): bell::Task("radio", 6 * 1024, 1)
{
    name = "webradio";
    audioStream = std::make_shared<HTTPAudioStream>();
}

void WebRadioPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader)
{
    scriptLoader->loadScript("webradio_plugin", berry);
}

void WebRadioPlugin::setupBindings()
{
    berry->export_this("webradioQueueUrl", this, &WebRadioPlugin::playRadioUrl);
}

void WebRadioPlugin::configurationUpdated()
{
}

void WebRadioPlugin::playRadioUrl(std::string url, bool isAAC)
{
    if (isAAC) {
        std::cout << "is aac" << std::endl;
    }
    isRunning = false;
    radioUrlQueue.push({isAAC, url});
}

void WebRadioPlugin::shutdown() {
    EUPH_LOG(info, "webradio", "Shutting down...");
    isRunning = false;
    std::lock_guard lock(runningMutex);
    status = ModuleStatus::SHUTDOWN;
}

void WebRadioPlugin::runTask()
{
    std::pair<bool, std::string> url;

    while (true) {
        if (this->radioUrlQueue.wpop(url)) {
            std::lock_guard lock(runningMutex);
            isRunning = true;
            status = ModuleStatus::RUNNING;

            EUPH_LOG(info, "webradio", "Starting WebRadio");
            // Shutdown all other modules
            audioBuffer->shutdownExcept(name);

            if (url.first) {
                audioStream->querySongFromUrl(url.second, AudioCodec::AAC);
            } else {
                audioStream->querySongFromUrl(url.second, AudioCodec::MP3);
            }
            while (isRunning) {
                audioStream->decodeFrame(audioBuffer);
                BELL_SLEEP_MS(10);
            }
        }
    }
}

void WebRadioPlugin::startAudioThread()
{
    startTask();
}