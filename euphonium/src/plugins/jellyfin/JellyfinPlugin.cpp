#include "JellyfinPlugin.h"
#include <thread>

JellyfinPlugin::JellyfinPlugin() : bell::Task("jellyfin", 6 * 1024, 3, 1) {
    name = "jellyfin";
    login_status = 0;
    audioStream = std::make_shared<JellyfinHTTPAudioStream>();
}

void JellyfinPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("jellyfin_plugin", berry);
}

void JellyfinPlugin::setupBindings() {
    berry->export_this("jellyfin_config_updated", this,
                       &JellyfinPlugin::configurationUpdated);
    berry->export_this("jellyfin_set_pause", this, &JellyfinPlugin::setPaused);
    berry->export_this("jellyfin_playSongByID", this,
                       &JellyfinPlugin::playSongByID);
    
}

void JellyfinPlugin::configurationUpdated() {
    // todo: load config
    url = "https://jellyfin.cf";
    username = "test";
    password = "dupa12"; // dummy account for testing
    // url = std::any_cast<std::string>(config["jellyfin_url"]);
    // username = std::any_cast<std::string>(config["jellyfin_username"]);
    // password = std::any_cast<std::string>(config["jellyfin_password"]);
    deviceName = "Euphonium";
    EUPH_LOG(info, "jellyfin", "Config updated");
    shutdown();
}

void JellyfinPlugin::setPaused(bool isPaused) {
    this->isPaused = isPaused;
    auto event = std::make_unique<PauseChangedEvent>(isPaused);
    this->luaEventBus->postEvent(std::move(event));
}

std::string JellyfinPlugin::jellyfinURL(std::string path) {
    std::string full_url;
    if (url.find("https://") == 0) { // if starts with https
        full_url += url;
    } else if(url.find("http://") == 0) { // if starts with http
        full_url += url;
    } else { // add https if not contains
        full_url += "http://";
        full_url += url;
    }
    return full_url + path;
}

void JellyfinPlugin::login() {
    if(username.empty() || password.empty()) {
        return;
    }

    client = std::make_shared<bell::HTTPClient>();
    
    // auth header
    std::string x_emby = "Emby UserId=\"" + username  +"\", Client=\"" + deviceName + "\", Device=\"" + deviceName + "\", DeviceId=\"" + deviceName + "\", Version=\"0.0.1\"";

    // prepare payload with username and password
    cJSON *payload = cJSON_CreateObject();
    cJSON_AddItemToObject(payload, "username", cJSON_CreateString(username.c_str()));
    cJSON_AddItemToObject(payload, "Pw", cJSON_CreateString(password.c_str()));
    char *payload_str = cJSON_Print(payload);
    cJSON_Delete(payload);

    // request
    struct bell::HTTPClient::HTTPRequest request = {
		.method = bell::HTTPClient::HTTPMethod::POST,
		.url = jellyfinURL("/Users/AuthenticateByName"),
		.body = payload_str,
		.contentType = "application/json",
		.headers = { {"X-Emby-Authorization", x_emby} },
		.maxRedirects = -1,
		.dumpFs = nullptr,
		.dumpRawFs = nullptr,
    };
    
    bell::HTTPClient::HTTPResponse_t response = client->execute(request);
    
    free(payload_str);
    
    cJSON *response_json = cJSON_Parse(response->readToString().c_str());
    if (response_json == NULL) {
        return;
    }
    
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(response_json, "AccessToken");
    if (!cJSON_IsString(token_json) || (token_json->valuestring == NULL)) {
        cJSON_Delete(response_json);
        return;
    }

   EUPH_LOG(info, "jellyfin", "Got token: %s", token_json->valuestring);
    token = token_json->valuestring;
    
    const cJSON *user_json = cJSON_GetObjectItemCaseSensitive(response_json, "User");
    if (user_json == NULL) {
        cJSON_Delete(response_json);
        return;
    }

    const cJSON *userid_json = cJSON_GetObjectItemCaseSensitive(user_json, "Id");
    if (!cJSON_IsString(userid_json) || (userid_json->valuestring == NULL)) {
        cJSON_Delete(response_json);
        return;
    }

    EUPH_LOG(info, "jellyfin", "Got user_id: %s", userid_json->valuestring);
    userid = userid_json->valuestring;
    
    cJSON_Delete(response_json);
    
    login_status = 1; // login successful
}

void JellyfinPlugin::updateSongInfo(std::string trackid){
    if(!login_status){
        return;
    }

    auto sourceName = std::string("jellyfin");

    // request
    struct bell::HTTPClient::HTTPRequest request = {
		.method = bell::HTTPClient::HTTPMethod::GET,
		.url = jellyfinURL("/Users/") + userid + std::string("/Items/") + trackid,
		.body = nullptr,
		.contentType = nullptr,
		.headers = { {std::string("x-mediabrowser-token"), token} },
		.maxRedirects = -1,
		.dumpFs = nullptr,
		.dumpRawFs = nullptr,
    };
    
    bell::HTTPClient::HTTPResponse_t response = client->execute(request);

    cJSON *response_json = cJSON_Parse(response->readToString().c_str());
    if (response_json == NULL) {
        return;
    }
    const cJSON *name_json = cJSON_GetObjectItemCaseSensitive(response_json, "Name");
    if (!cJSON_IsString(name_json) || (name_json->valuestring == NULL)) {
        cJSON_Delete(response_json);
        return;
    }
    const cJSON *album_json = cJSON_GetObjectItemCaseSensitive(response_json, "Album");
    if (!cJSON_IsString(album_json) || (album_json->valuestring == NULL)) {
        cJSON_Delete(response_json);
        return;
    }
    const cJSON *albumartist_json = cJSON_GetObjectItemCaseSensitive(response_json, "AlbumArtist");
    if (!cJSON_IsString(albumartist_json) || (albumartist_json->valuestring == NULL)) {
        cJSON_Delete(response_json);
        return;
    }

    std::string imageUrl = jellyfinURL("/Items/") + trackid + "/Images/Primary?fillHeight=200&fillWidth=200&quality=96";
    
    auto event_track = std::make_unique<SongChangedEvent>(
        name_json->valuestring, album_json->valuestring, albumartist_json->valuestring, sourceName, imageUrl);
    this->luaEventBus->postEvent(std::move(event_track));
    
    auto event_state = std::make_unique<PauseChangedEvent>(false);
    this->luaEventBus->postEvent(std::move(event_state));

    EUPH_LOG(info, "jellyfin", "Song name changed");
    cJSON_Delete(response_json);
}

void JellyfinPlugin::playSongByID(std::string trackid) {
    if(!login_status){
        return;
    }
    isRunning = false;
    std::map<std::string, std::string> headers = { {std::string("x-mediabrowser-token"), token} };

    std::string args;
    args += std::string("?MaxStreamingBitrate=") + std::to_string(128*1024);
    args += std::string("&api_key=") + token;
    args += std::string("&UserId=") + userid;
    args += std::string("&DeviceId=") + deviceName;
    args += std::string("&PlaySessionId=") + std::string("WIeQFWTdqmMyMXONJhBd"); // hardcoded for now
    args += std::string("&TranscodingProtocol=") + std::string("http");
    args += std::string("&Container=") + std::string("mp3");
    args += std::string("&AudioCodec=") + std::string("mp3");
    args += std::string("&CopyTimestamps=") + std::string("true");

    updateSongInfo(trackid);
    radioUrlQueue.push({headers, jellyfinURL("/Audio/") + trackid + "/universal" +args});
}

void JellyfinPlugin::shutdown() {
    EUPH_LOG(info, "jellyfin", "Shutting down...");
    isRunning = false;
    std::lock_guard lock(runningMutex);
    status = ModuleStatus::SHUTDOWN;
}

void JellyfinPlugin::runTask() {
    configurationUpdated();    
    login();

    std::pair<std::map<std::string, std::string>, std::string> url;

    while (true) {
        if (this->radioUrlQueue.wpop(url)) {
            std::lock_guard lock(runningMutex);
            isRunning = true;
            isPaused = false;

            EUPH_LOG(info, "jellyfin", "Starting Jellyfin");
            // Shutdown all other modules
            audioBuffer->shutdownExcept(name);
            audioBuffer->lockAccess();
            EUPH_LOG(info, "jellyfin", "Got access to audio buffer");
            audioBuffer->configureOutput(AudioOutput::SampleFormat::INT16,
                                         44100);
            status = ModuleStatus::RUNNING;
            try {
                audioStream->querySongFromUrl(url.second, JellyfinAudioCodec::MP3, url.first);
                
                while (isRunning) {
                    if (!isPaused) {
                        audioStream->decodeFrame(audioBuffer);

                        // Change sample rate if necessary
                        if (audioStream->currentSampleRate !=
                                audioBuffer->sampleRate &&
                            audioStream->currentSampleRate != 0) {
                            audioBuffer->configureOutput(
                                AudioOutput::SampleFormat::INT16,
                                audioStream->currentSampleRate);
                        }

                        BELL_YIELD();
                    } else {
                        BELL_SLEEP_MS(100);
                    }
                }
            } catch (...) {
                BELL_LOG(error, "jellyfin", "Cannot play requested radio");
                auto source = std::string("jellyfin");
                auto error = std::string("Cannot play requested track");
                auto event = std::make_unique<PlaybackError>(source, error);
                this->luaEventBus->postEvent(std::move(event));
            }
            mainAudioBuffer->unlockAccess();
        }
    }
}

void JellyfinPlugin::startAudioThread() { startTask(); }
