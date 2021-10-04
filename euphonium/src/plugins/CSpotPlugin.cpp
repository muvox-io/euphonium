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

void audioThread()
{
    configMan->format = AudioFormat::OGG_VORBIS_320;
    configMan->deviceName = "CSpot (euphonium)";
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
        auto audioSink = std::make_shared<FakeAudioSink>();
        auto spircController = std::make_shared<SpircController>(mercuryManager, blob->username, audioSink);
        mercuryManager->reconnectedCallback = [spircController]()
        {
            return spircController->subscribe();
        };
        mercuryManager->handleQueue();
    }
}

CSpotPlugin::CSpotPlugin()
{
    auto file = std::make_shared<CliFile>();
    configMan = std::make_shared<ConfigJSON>("test.json", file);
    name = "cspot";
}

void CSpotPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader, std::shared_ptr<sol::state> luaState)
{
    scriptLoader->loadScript("cspot_plugin", luaState);
}

void CSpotPlugin::setupLuaBindings(std::shared_ptr<sol::state> luaState)
{
}

void CSpotPlugin::startAudioThread()
{
    std::thread newThread(audioThread);
    newThread.detach();
    // newThread.join();
}