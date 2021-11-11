#include "CSpotPlugin.h"

#include <thread>

std::shared_ptr<ConfigJSON> configMan;

CSpotPlugin::CSpotPlugin()
{
    auto file = std::make_shared<CliFile>();
    configMan = std::make_shared<ConfigJSON>("test.json", file);
    name = "cspot";
    cspotGlobalLogger = std::make_shared<CSpotEuphLogger>();
}

void CSpotPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader)
{
    scriptLoader->loadScript("cspot_plugin", luaState);
}

void CSpotPlugin::setupLuaBindings()
{
    sol::state_view lua(luaState);
    lua.set_function("cspotConfigUpdated", &CSpotPlugin::configurationUpdated, this);
}

void CSpotPlugin::configurationUpdated()
{
    std::cout << "CSpotPlugin::configurationUpdated()" << std::endl;
    this->isRunning = false;
    std::scoped_lock(this->runningMutex);
    mercuryManager->stop();
    spircController->stopPlayer();
    usleep(500000);
    spircController.reset();
    mercuryManager.reset();
    usleep(500000);
    //startAudioThread();
}

void CSpotPlugin::startCSpot()
{
    std::scoped_lock lock(runningMutex);
    this->isRunning = true;

    if (authBlob == nullptr)
    {
        auto authenticator = std::make_shared<ZeroconfAuthenticator>();
        authBlob = authenticator->listenForRequests();
    }

    auto session = std::make_unique<Session>();
    session->connectWithRandomAp();
    auto token = session->authenticate(authBlob);
    if (token.size() > 0)
    {
        // @TODO Actually store this token somewhere
        mercuryManager = std::make_shared<MercuryManager>(std::move(session));
        mercuryManager->startTask();
        auto audioSink = std::make_shared<FakeAudioSink>(this->audioBuffer);
        spircController = std::make_shared<SpircController>(mercuryManager, authBlob->username, audioSink);
        spircController->setTrackChangedCallback([this](TrackInfo &track) {
            auto sourceName = std::string("cspot");
            auto event = std::make_unique<SongChangedEvent>(track.name, track.album, track.artist, sourceName);
            EUPH_LOG(info, "cspot", "Song name changed");
            this->luaEventBus->postEvent(std::move(event));
        });

        mercuryManager->reconnectedCallback = [this]()
        {
            return this->spircController->subscribe();
        };

        while (this->isRunning)
        {
            mercuryManager->updateQueue();
        }
        std::cout << "DONE" << std::endl;
    }
}

void CSpotPlugin::mapConfig()
{
    configMan->deviceName = config["receiverName"];
    std::string bitrateString = config["audioBitrate"];
    switch (std::stoi(bitrateString))
    {
    case 160:
        configMan->format = AudioFormat::OGG_VORBIS_160;
        break;
    case 96:
        configMan->format = AudioFormat::OGG_VORBIS_96;
        break;
    default:
        configMan->format = AudioFormat::OGG_VORBIS_320;
        break;
    }
}

void CSpotPlugin::startAudioThread()
{
    this->mapConfig();
    std::thread newThread(&CSpotPlugin::startCSpot, this);
    newThread.detach();
}