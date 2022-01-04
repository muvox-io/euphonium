#ifndef EUPHONIUM_ESP32_BLUETOOTH_H
#define EUPHONIUM_ESP32_BLUETOOTH_H

#include "ScriptLoader.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include "Core.h"
#include "CoreEvents.h"
#include <memory>
#include "I2CDriver.h"
#include "I2SDriver.h"
#include "Task.h"

extern "C" {
#include "bt_app_sink.h"
}
#include "WiFiDriver.h"

class BluetoothPlugin : public bell::Task, public Module
{


public:
    BluetoothPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown();
    void configurationUpdated() {};
    void startAudioThread();
    void runTask();
};

#endif
