#ifndef EUPHONIUM_ESP32_PLATFORM_H
#define EUPHONIUM_ESP32_PLATFORM_H

#include "ScriptLoader.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include "I2CDriver.h"
#include <freertos/task.h>
#include "I2SDriver.h"
#include "GPIODriver.h"
#include "Task.h"
#include "WiFiDriver.h"

class ESP32PlatformPlugin : public bell::Task, public Module
{

public:
    ESP32PlatformPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown() {};
    std::string dumpTaskInfo();
    void reportRAM(std::string);
    void configurationUpdated() {};
    void startAudioThread() {};
    void runTask() {};
};

#endif
