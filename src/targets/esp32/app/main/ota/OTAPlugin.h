
#ifndef EUPHONIUM_ESP32_OTA_H
#define EUPHONIUM_ESP32_OTA_H

#include "ScriptLoader.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include "Task.h"
#include <esp_ota_ops.h>
#include "esp_partition.h"

class OTAPlugin : public bell::Task, public Module
{

public:
    OTAPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown() {};
    void rebootToRecovery();
    void configurationUpdated() {};
    void startAudioThread() {};
    void runTask() {};
};

#endif
