#include "CSpotPlugin.h"
#include "ConfigJSON.h"
#include "MercuryManager.h"
#include "ZeroconfAuthenticator.h"
#include "Session.h"
#include "SpircController.h"
#include "FakeAudioSink.h"
#include "CliFile.h"
#include <thread>

std::shared_ptr<ConfigJSON> configMan;

CSpotPlugin::CSpotPlugin()
{
    auto file = std::make_shared<CliFile>();
    configMan = std::make_shared<ConfigJSON>("test.json", file);
    name = "cspot";
}

void CSpotPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader)
{
    scriptLoader->loadScript("cspot_plugin", luaState);
}

void CSpotPlugin::setupLuaBindings()
{
}

void CSpotPlugin::startCSpot() {
    auto authenticator = std::make_shared<ZeroconfAuthenticator>();
    auto blob = authenticator->listenForRequests();
    auto session = std::make_unique<Session>();
    session->connectWithRandomAp();
    auto token = session->authenticate(blob);
    if (token.size() > 0)
    {
        // @TODO Actually store this token somewhere
        auto mercuryManager = std::make_shared<MercuryManager>(std::move(session));
        mercuryManager->startTask();
        auto audioSink = std::make_shared<FakeAudioSink>(this->audioBuffer);
        auto spircController = std::make_shared<SpircController>(mercuryManager, blob->username, audioSink);
        mercuryManager->reconnectedCallback = [spircController]()
        {
            return spircController->subscribe();
        };
        mercuryManager->handleQueue();
    }
}

void CSpotPlugin::startAudioThread()
{
    configMan->deviceName = config["receiverName"];
    std::string bitrateString = config["audioBitrate"];
    switch(std::stoi(bitrateString)) {
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

    std::thread newThread(&CSpotPlugin::startCSpot, this);
    newThread.detach();
}