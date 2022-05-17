#ifndef EUPHONIUM_ESP32_PLATFORM_H
#define EUPHONIUM_ESP32_PLATFORM_H

#include <map>
#include <vector>
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
#include "BellUtils.h"
#include "driver/gpio.h"
#include "WiFiDriver.h"
#include "LEDDriver.h"
#include "Rotary.h"

#define BUTTON_DEBOUNCE_US 5000

class ButtonInteractionEvent : public Event {
  private:
    std::string type;
    int buttonNumber;

  public:
    ButtonInteractionEvent(std::string type, int buttonNumber) {
        this->type = type;
        this->buttonNumber = buttonNumber;
        this->subType = "buttonInteractionEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["type"] = this->type;
        result["button"] = this->buttonNumber;
        return result;
    }
};
extern IRAM_ATTR std::vector<Rotary> registeredEncodersList;

enum class ButtonEventType : uint8_t {
    BUTTON = 0,
    ENCODER_CW = 1,
    ENCODER_CCW = 2
};

struct InterruptEvent {
    ButtonEventType buttonEventType;
    uint32_t gpioNumber;
};

class ESP32PlatformPlugin : public bell::Task, public Module
{
private:
    std::map<int, std::pair<bool, bool>> buttonStateMap;
    std::vector<int> rawButtonList;
    std::vector<int> buttonList;

public:
    ESP32PlatformPlugin();
    
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown() {};
    std::string dumpTaskInfo();
    void reportRAM();
    void configurationUpdated() {};
    void registerButtonRaw(int gpio);
    void registerEncoder(int gpioA, int gpioB);
    void registerButton(int gpio);
    void startAudioThread() {
        startTask();
    };
    void runTask();
};

#endif
