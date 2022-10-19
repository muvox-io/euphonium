#ifndef EUPHONIUM_ESP32_BLUETOOTH_H
#define EUPHONIUM_ESP32_BLUETOOTH_H

#include "Core.h"
#include "CoreEvents.h"
#include "EuphoniumLog.h"
#include "I2CDriver.h"
#include "I2SDriver.h"
#include "Logger.h"
#include "Module.h"
#include "Queue.h"
#include "ScriptLoader.h"
#include "Task.h"
#include "WiFiDriver.h"
#include <memory>
#include "BluetoothDriver.h"
class BluetoothPlugin : public bell::Task, public Module {

  private:
    enum class BTEvent {
        Initialize,
        Deinitialize,
        Disconnect,
        LockAccess,
    };
    bell::Queue<BTEvent> btEventQueue;
    std::shared_ptr<BluetoothDriver> btDriver;

  public:
    BluetoothPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown();
    void configurationUpdated(){};
    void startAudioThread();
    void runTask();
    void setStatus(ModuleStatus status);
};

#endif
