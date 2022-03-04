#ifndef EUPH_SNAPCAST_PLUGIN_H
#define EUPH_SNAPCAST_PLUGIN_H

#include "ScriptLoader.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include "Core.h"
#include <atomic>
#include <mutex>
#include "Queue.h"
#include "Task.h"
#include "SnapcastConnection.h"

class SnapcastPlugin : public bell::Task, public Module
{
  private:
    std::unique_ptr<Snapcast::Connection> connection;
  public:
    SnapcastPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown();
    void configurationUpdated();
    void startAudioThread();
    void runTask();
};

#endif
