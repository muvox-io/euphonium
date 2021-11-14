#ifndef EUPHONIUM_CSPOT_PLUGIN_H
#define EUPHONIUM_CSPOT_PLUGIN_H

#include "ScriptLoader.h"
#include <sol.hpp>
#include "ConfigJSON.h"
#include "MercuryManager.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "ZeroconfAuthenticator.h"
#include "Session.h"
#include "CoreEvents.h"
#include "SpircController.h"
#include "FakeAudioSink.h"
#include "CliFile.h"
#include "Module.h"
#include <atomic>
#include <mutex>
class CSpotPlugin : public Module
{
private:
    std::shared_ptr<MercuryManager> mercuryManager;
    std::shared_ptr<SpircController> spircController;
    std::shared_ptr<FakeAudioSink> fakeAudioSink;
    std::atomic<bool> isRunning = false;
    std::mutex runningMutex;
    std::shared_ptr<LoginBlob> authBlob;
    void mapConfig();

public:
    CSpotPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupLuaBindings();
    void startCSpot();
    void configurationUpdated();
    void startAudioThread();
};

#endif