WebRadioPlugin = {name = "webradio"}
WebRadioPlugin.__index = WebRadioPlugin

function WebRadioPlugin.new()
    local self = setmetatable({name = "webradio", displayName = "Web Radio", type="plugin"}, WebRadioPlugin)

    -- Plugin configuration schema
    self.configScheme = {
        stationUrl = {
            tooltip = "Station's url",
            type = "string",
            defaultValue = "none"
        },
        stationCodec = {
            tooltip = "Station codec",
            type = "stringList",
            listValues = {"AAC", "MP3", "OPUS"},
            defaultValue = "AAC"
        }
    }
    return self
end

function WebRadioPlugin.configurationChanged(self)
    logInfo("Configuration changed")
    startAudioThreadForPlugin("webradio", getBareConfig(self.configScheme))
end

http:handle(http.POST, "/webradio", function(request)
    local body = json.decode(request.body)
    local response = {
        status = "playing"
    }
    
    webradioQueueUrl(body.stationUrl, body["codec"] == "AAC" or body["codec"] == "AAC+")
    http:sendJSON(response, request.connection)
end)

app:registerPlugin(WebRadioPlugin.new())