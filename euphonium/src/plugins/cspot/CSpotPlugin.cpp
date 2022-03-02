#include "CSpotPlugin.h"
#ifdef ESP_PLATFORM
#include "mdns.h"
#endif

#include <thread>

std::shared_ptr<ConfigJSON> configMan;

CSpotPlugin::CSpotPlugin() : bell::Task("cspot", 4 * 1024, 0, 0) {
    auto file = std::make_shared<CliFile>();
    configMan = std::make_shared<ConfigJSON>("", file);
    name = "cspot";
#ifdef ESP_PLATFORM
    // setup mdns
    mdns_init();
    mdns_hostname_set("cspot");
#endif
}

void CSpotPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("cspot_plugin", berry);
}

void CSpotPlugin::setupBindings() {
    berry->export_this("cspot_config_updated", this,
                       &CSpotPlugin::configurationUpdated);
    berry->export_this("cspot_set_pause", this, &CSpotPlugin::setPause);
    berry->export_this("cspot_set_volume_remote", this,
                       &CSpotPlugin::setVolumeRemote);
}

void CSpotPlugin::setPause(bool pause) {
    if (spircController != nullptr) {
        spircController->setPause(pause);
    }
}

void CSpotPlugin::setVolumeRemote(int volume) {
    double volStep = MAX_VOLUME / 100.0;
    auto mappedVolume = ((int)volStep * volume);

    if (spircController != nullptr) {
        spircController->setRemoteVolume(mappedVolume);
    }

    configMan->volume = mappedVolume;
}

void CSpotPlugin::configurationUpdated() {
    mapConfig();
    shutdown();
}

void CSpotPlugin::shutdown() {
    this->isRunning = false;
    std::scoped_lock(this->runningMutex);
    spircController->stopPlayer();

    mercuryManager->stop();
    BELL_SLEEP_MS(100);
    spircController.reset();
    mercuryManager.reset();
    BELL_SLEEP_MS(100);
    status = ModuleStatus::SHUTDOWN;
}

void CSpotPlugin::runTask() {
    status = ModuleStatus::RUNNING;
    std::scoped_lock lock(runningMutex);
    this->isRunning = true;

    EUPH_LOG(info, "cspot", "Starting CSpot");
    this->audioBuffer->shutdownExcept(name);
    this->audioBuffer->lockAccess();
    this->audioBuffer->configureOutput(AudioOutput::SampleFormat::INT16, 44100);
    BELL_SLEEP_MS(1000);

    auto session = std::make_unique<Session>();
    session->connectWithRandomAp();

    EUPH_LOG(info, "cspot", "Got session");
    auto token = session->authenticate(authBlob);

    EUPH_LOG(info, "cspot", "Auth");
    if (token.size() > 0) {
        // @TODO Actually store this token somewhere
        mercuryManager = std::make_shared<MercuryManager>(std::move(session));
        mercuryManager->startTask();
        auto audioSink = std::make_shared<FakeAudioSink>(this->audioBuffer,
                                                         this->luaEventBus);
        spircController = std::make_shared<SpircController>(
            mercuryManager, authBlob->username, audioSink);

        // Add event handler
        spircController->setEventHandler([this](CSpotEvent &event) {
            switch (event.eventType) {
            case CSpotEventType::TRACK_INFO: {
                // Handle track info
                TrackInfo track = std::get<TrackInfo>(event.data);
                auto sourceName = std::string("cspot");
                auto event = std::make_unique<SongChangedEvent>(
                    track.name, track.album, track.artist, sourceName,
                    track.imageUrl);
                EUPH_LOG(info, "cspot", "Song name changed");
                this->luaEventBus->postEvent(std::move(event));
                this->audioBuffer->clearBuffer();
                break;
            }
            case CSpotEventType::PLAY_PAUSE: {
                // Handle stop pause
                bool isPaused = std::get<bool>(event.data);
                auto event = std::make_unique<PauseChangedEvent>(isPaused);
                this->luaEventBus->postEvent(std::move(event));
                this->audioBuffer->clearBuffer();
                break;
            }
            case CSpotEventType::SEEK: {
                this->audioBuffer->clearBuffer();
                break;
            }
            default:
                break;
            }
        });

        mercuryManager->reconnectedCallback = [this]() {
            return this->spircController->subscribe();
        };

        while (this->isRunning) {
            mercuryManager->updateQueue();
        }
        BELL_LOG(info, "cspot", "Unlocking audio mutex");
        audioBuffer->unlockAccess();
    }
}

void CSpotPlugin::mapConfig() {
    configMan->deviceName = std::any_cast<std::string>(config["receiverName"]);
    std::string bitrateString =
        std::any_cast<std::string>(config["audioBitrate"]);
    switch (std::stoi(bitrateString)) {
    case 160:
        configMan->format = AudioFormat_OGG_VORBIS_160;
        break;
    case 96:
        configMan->format = AudioFormat_OGG_VORBIS_96;
        break;
    default:
        configMan->format = AudioFormat_OGG_VORBIS_320;
        break;
    }
}

void CSpotPlugin::startAudioThread() {
    mapConfig();
    if (this->authenticator == nullptr) {
        createPlayerCallback = [this](std::shared_ptr<LoginBlob> blob) {
            if (this->isRunning) {
                configurationUpdated();
            }

            this->authBlob = blob;
            EUPH_LOG(info, "cspot", "Authenticated");
            startTask();
            EUPH_LOG(info, "cspot", "Detached");
        };

        BELL_LOG(info, "cspot", "Setting up zeroconf");
        authenticator = std::make_shared<ZeroconfAuthenticator>(
            createPlayerCallback, mainServer);
        authenticator->registerHandlers();
    }
}
