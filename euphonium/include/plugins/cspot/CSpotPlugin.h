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

class CSpotEuphLogger : public AbstractLogger
{
public:
    CSpotEuphLogger() {};
    void debug(std::string filename, int line, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        euphoniumLogger->debug(filename, line, "cspot", format, args);
        va_end(args);
    };

    void error(std::string filename, int line, const char *format, ...)
    {
        va_list argp;
        va_start(argp, format);
        euphoniumLogger->error(filename, line, "cspot", format, argp);
        va_end(argp);
    };

    void info(std::string filename, int line, const char *format, ...)
    {
        va_list argp;
        va_start(argp, format);
        euphoniumLogger->info(filename, line, "cspot", format, argp);
        va_end(argp);
    };
};
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