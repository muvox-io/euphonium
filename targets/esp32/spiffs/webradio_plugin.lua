WebRadioPlugin = {name = "webradio"}
WebRadioPlugin.__index = WebRadioPlugin

function WebRadioPlugin.new()
    local self = setmetatable({name = "webradio", displayName = "Web Radio", type="plugin", registerWebApp = true}, WebRadioPlugin)

    -- Plugin configuration schema
    self.configScheme = {
        stationUrl = {
            tooltip = "RadioBrowser instance",
            type = "string",
            defaultValue = "https://de1.api.radio-browser.info/"
        },
    }
    return self
end

function WebRadioPlugin.configurationChanged(self)
    logInfo("Configuration changed")
    startAudioThreadForPlugin("webradio", getBareConfig(self.configScheme))
end

http:handle(http.POST, "/webradio", function(request)
    local body = json.decode(request.body)
    webradioQueueUrl(body.stationUrl, body["codec"] == "AAC" or body["codec"] == "AAC+")
    http:sendJSON({ status = "playing" }, request.connection)

    -- update playback status
    local song = {
        songName = body["stationName"],
        artistName = "Internet Radio",
        sourceName = "webradio",
        icon = body["favicon"],
        albumName = body["codec"]
    }
    handleSongChangedEvent(song)
end)

app:registerPlugin(WebRadioPlugin.new())