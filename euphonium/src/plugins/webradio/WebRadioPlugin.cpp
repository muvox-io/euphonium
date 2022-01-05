#include "WebRadioPlugin.h"
#include <HTTPStream.h>
#include <thread>

WebRadioPlugin::WebRadioPlugin() : bell::Task("radio", 6 * 1024, 1, 1) {
    name = "webradio";
    audioStream = std::make_shared<HTTPAudioStream>();
}

void WebRadioPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("webradio_plugin", berry);
}

void WebRadioPlugin::setupBindings() {
    berry->export_this("webradio_set_pause", this, &WebRadioPlugin::setPaused);
    berry->export_this("webradio_queue_url", this,
                       &WebRadioPlugin::playRadioUrl);
}

void WebRadioPlugin::configurationUpdated() {}

void WebRadioPlugin::setPaused(bool isPaused) {
    this->isPaused = isPaused;
    auto event = std::make_unique<PauseChangedEvent>(isPaused);
    this->luaEventBus->postEvent(std::move(event));
}

void WebRadioPlugin::playRadioUrl(std::string url, bool isAAC) {
    isRunning = false;
    radioUrlQueue.push({isAAC, url});
}

void WebRadioPlugin::shutdown() {
    EUPH_LOG(info, "webradio", "Shutting down...");
    isRunning = false;
    std::lock_guard lock(runningMutex);
    status = ModuleStatus::SHUTDOWN;
}

void WebRadioPlugin::runTask() {
    std::pair<bool, std::string> url;

    while (true) {
        if (this->radioUrlQueue.wpop(url)) {
            std::lock_guard lock(runningMutex);
            isRunning = true;
            isPaused = false;
            status = ModuleStatus::RUNNING;

            EUPH_LOG(info, "webradio", "Starting WebRadio");
            // Shutdown all other modules
            audioBuffer->shutdownExcept(name);
            audioBuffer->lockAccess();
            audioBuffer->configureOutput(AudioOutput::SampleFormat::INT16,
                                         44100);

            try {
                if (url.first) {
                    audioStream->querySongFromUrl(url.second, AudioCodec::AAC);
                } else {
                    audioStream->querySongFromUrl(url.second, AudioCodec::MP3);
                }

                while (isRunning) {
                    if (!isPaused) {
                        audioStream->decodeFrame(audioBuffer);

                        // Change sample rate if necessary
                        if (audioStream->currentSampleRate !=
                                audioBuffer->sampleRate &&
                            audioStream->currentSampleRate != 0) {
                            audioBuffer->configureOutput(
                                AudioOutput::SampleFormat::INT16,
                                audioStream->currentSampleRate);
                        }

                        BELL_YIELD();
                    } else {
                        BELL_SLEEP_MS(100);
                    }
                }
            } catch (...) {
                BELL_LOG(error, "webradio", "Cannot play requested radio");
                auto source = std::string("webradio");
                auto error = std::string("Cannot play requested station");
                auto event = std::make_unique<PlaybackError>(source, error);
                this->luaEventBus->postEvent(std::move(event));
            }
            mainAudioBuffer->unlockAccess();
        }
    }
}

void WebRadioPlugin::startAudioThread() { startTask(); }
