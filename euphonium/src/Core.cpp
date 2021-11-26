#include "Core.h"
#include <string.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>
#include <cassert>
#include <EuphoniumLog.h>

int my_add_func(bvm *vm)
{
    /* check the arguments are all integers */
    if (be_isint(vm, 1) && be_isint(vm, 2)) {
        bint a = be_toint(vm, 1); /* get the first argument */
        bint b = be_toint(vm, 2); /* get the second argument */
        be_pushint(vm, a + b); /* push the result to the stack */
    } else if (be_isnumber(vm, 1) && be_isnumber(vm, 2)) { /* check the arguments are all numbers */
        breal a = be_toreal(vm, 1); /* get the first argument */
        breal b = be_toreal(vm, 1); /* get the second argument */
        be_pushreal(vm, a + b); /* push the result to the stack */
    } else { /* unacceptable parameters */
        be_pushnil(vm); /* push the nil to the stack */
    }
    be_return(vm); /* return calculation result */
}

Core::Core() : bell::Task("Core", 4 * 1024, 0, false) {
    // beVm = be_vm_new();
    // be_regfunc(beVm, "add", my_add_func);
    // be_loadstring(beVm, "print(add(1, 2))");
    // be_pcall(beVm, 0);

    // return;
    audioBuffer = std::make_shared<MainAudioBuffer>();
    luaEventBus = std::make_shared<EventBus>();
    audioProcessor = std::make_shared<AudioProcessors>();
    audioProcessor->addProcessor(std::make_unique<SoftwareVolumeProcessor>());
    audioProcessor->addProcessor(std::make_unique<EqualizerProcessor>());

    // Prepare lua event thread
    auto subscriber = dynamic_cast<EventSubscriber*>(this);
    luaEventBus->addListener(EventType::LUA_MAIN_EVENT, *subscriber);

    this->setupBindings();
    registeredPlugins = {
        std::make_shared<CSpotPlugin>(),
        std::make_shared<WebRadioPlugin>(),
        std::make_shared<YouTubePlugin>()
    };
    requiredModules = {
        std::make_shared<HTTPModule>()};

    audioBuffer->shutdownListener = [this](std::string exceptPlugin) {
        for (auto& plugin : registeredPlugins) {
            if (plugin->name != exceptPlugin && plugin->status == ModuleStatus::RUNNING) {
                EUPH_LOG(info, "core", "Shutting down %s", plugin->name.c_str());
                plugin->shutdown();
            }
        }
    };
}

void checkResult(sol::protected_function_result result)
{
    if (!result.valid())
    {
        std::cout << ((sol::error)result).what() << std::endl;
    }
}

void Core::loadPlugins(std::shared_ptr<ScriptLoader> loader)
{
    luaState.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::debug);
    std::vector<std::string> luaModules({"json", "app"});

    audioProcessor->setLuaBindings(luaState);

    for (auto const &module : this->requiredModules)
    {
        EUPH_LOG(info, module->name, "Initializing");
        module->luaEventBus = this->luaEventBus;
        module->luaState = luaState;
        module->setupLuaBindings();
        module->loadScript(loader);
    }

    for (auto const &value : luaModules)
    {
        loader->loadScript(value, luaState);
    }

    for (auto const &plugin : this->registeredPlugins)
    {
        EUPH_LOG(info, plugin->name, "Initializing");
        plugin->luaState = this->luaState;
        plugin->luaEventBus = this->luaEventBus;
        plugin->setupLuaBindings();
        plugin->loadScript(loader);
    }
    
    checkResult(luaState.script("app:printRegisteredPlugins()"));

    startTask();

    EUPH_LOG(info, "core", "Lua thread listening");
    while(true) {
        BELL_SLEEP_MS(100);
        luaEventBus->update();
    }
}

void Core::selectAudioOutput(std::shared_ptr<AudioOutput> output)
{
    currentOutput = output;
    this->outputConnected = true;
}

void Core::handleEvent(std::unique_ptr<Event> event) {
    EUPH_LOG(debug, "core", "Got event");
    luaState["handleEvent"](event->luaEventType, event->toLua(luaState));
}

void Core::startAudioThreadForPlugin(std::string pluginName, sol::table config) {
    for (auto const &plugin : this->registeredPlugins) {
        if (plugin->name == pluginName) {
            EUPH_LOG(info, plugin->name, "Starting audio thread");
            plugin->config = config;
            plugin->audioBuffer = audioBuffer;
            plugin->startAudioThread();
            return;
        }
    }

    for (auto const &module : this->requiredModules) {
        if (module->name == pluginName) {
            EUPH_LOG(info, module->name, "Starting audio thread");
            module->config = config;
            module->audioBuffer = audioBuffer;
            module->startAudioThread();
            return;
        }
    }
}

void Core::setupBindings() {
    luaState.set_function("startAudioThreadForPlugin", &Core::startAudioThreadForPlugin, this);
    luaState.set_function("luaLogError", luaLogError);
    luaState.set_function("luaLogDebug", luaLogDebug);
    luaState.set_function("luaLogInfo", luaLogInfo);
    luaState.set_function("luaLog", luaLog);

    sol::usertype<PlaybackInfo> playbackType = luaState.new_usertype<PlaybackInfo>("PlaybackInfo", sol::constructors<PlaybackInfo()>());
    playbackType["albumName"] = &PlaybackInfo::albumName;
    playbackType["artistName"] = &PlaybackInfo::artistName;
    playbackType["icon"] = &PlaybackInfo::icon;
    playbackType["songName"] = &PlaybackInfo::songName;
    playbackType["sourceName"] = &PlaybackInfo::sourceName;
}

void Core::runTask() {
    EUPH_LOG(info, "core", "Audio output started");
    std::vector<uint8_t> pcmBuf(PCMBUF_SIZE);

    while (true) {
        if (audioBuffer->audioBuffer->size() > 0 && outputConnected) {
            auto readNumber = audioBuffer->audioBuffer->read(pcmBuf.data(), PCMBUF_SIZE);
            audioProcessor->process(pcmBuf.data(), readNumber);
            currentOutput->feedPCMFrames(pcmBuf.data(), readNumber);
        } else {
            EUPH_LOG(info, "core", "No data");
            BELL_SLEEP_MS(1000);
        }
    }
}
