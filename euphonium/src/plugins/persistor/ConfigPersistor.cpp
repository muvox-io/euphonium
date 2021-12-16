#include "ConfigPersistor.h"

ConfigPersistor::ConfigPersistor() : bell::Task("persistor",  4 * 1024, 0, false)
{
    name = "persistor";
}

void ConfigPersistor::loadScript(std::shared_ptr<ScriptLoader> loader)
{
    //    loader->loadScript("http", berry);
    this->scriptLoader = loader;
}

void ConfigPersistor::persist(std::string key, std::string value)
{
    PersistenceRequest request = {
        .isSave = true,
        .key = key,
        .value = value};

    requestQueue.push(request);
}

void ConfigPersistor::load(std::string key)
{
    PersistenceRequest request = {
        .isSave = false,
        .key = key,
        .value = ""};

    requestQueue.push(request);
}

void ConfigPersistor::setupBindings()
{
    BELL_LOG(info, "persistor", "Registering handlers");
    berry->export_this("conf_persist", this, &ConfigPersistor::persist);
    berry->export_this("conf_load", this, &ConfigPersistor::load);
}

void ConfigPersistor::runTask()
{
    PersistenceRequest request;
    while (true)
    {
        if (this->requestQueue.wpop(request))
        {
            // save request.value to file named key
            if (request.isSave)
            {
                BELL_LOG(info, "persistor", "Saving key: %s", request.key.c_str());
                scriptLoader->saveFile(request.key + ".config.json", request.value);
            }
            else
            {
                std::string value = scriptLoader->loadFile(request.key + ".config.json");
                BELL_LOG(info, "persistor", "Loaded key: %s", request.key.c_str());
                auto event = std::make_unique<ConfigLoadedEvent>(request.key, value);
                EUPH_LOG(info, "persistor", "Posting the event");
                this->luaEventBus->postEvent(std::move(event));
            }
        }
    }
}

void ConfigPersistor::startAudioThread()
{
    startTask();
}
