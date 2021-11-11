App = {
    plugins = {}
}
App.__index = App
http = Http.new()


function handleRouteEvent(request)
    http:handleIncomingRequest(request)
end

function handleSongChangedEvent(song)
    logInfo("Song changed: ")
    app.playbackState.songName = song.songName
    app.playbackState.artistName = song.artistName
    app.playbackState.sourceName = song.sourceName
    app.playbackState.albumName = song.albumName
end

function App.new()
    local self = setmetatable({
        plugins = {},
        eventHandlers = {
            handleRouteEvent = handleRouteEvent,
            songChangedEvent = handleSongChangedEvent
        },
        playbackState = {
            songName = "",
            artistName = "",
            sourceName = "",
            albumName = ""
        }
    }, App)
    return self
end

function App.registerPlugin(self, plugin)
    self.plugins[plugin.name] = plugin
end

function App.registerHandler(self, eventType, handler)
    self.eventHandlers[eventType] = handler
end

function App.handleEvent(self, eventType, eventData)
    if self.eventHandlers[eventType] then
        self.eventHandlers[eventType](eventData)
    end
end

function logInfo(msg)
    luaLogInfo(debug.getinfo(2,'S').source, debug.getinfo(2, 'l').currentline, msg)
end


function logDebug(msg)
    luaLogDebug(debug.getinfo(2,'S').source, debug.getinfo(2, 'l').currentline, msg)
end


function logError(msg)
    luaLogError(debug.getinfo(2,'S').source, debug.getinfo(2, 'l').currentline, msg)
end

function handleEvent(eventType, eventData)
    logDebug("Received event: " .. eventType)
    app:handleEvent(eventType, eventData)
end

function getBareConfig(configSchema) 
    bareConfig = {}
    for k, v in pairs(configSchema) do
        if v.value == nil then
            v.value = v.defaultValue
        end

        bareConfig[k] = v.value
    end

    return bareConfig
end

function App.enablePlugin(plugin)
    bareConfig = {}

    if app.plugins[plugin].configScheme then
        bareConfig = getBareConfig(app.plugins[plugin].configScheme)
    end
    startAudioThreadForPlugin(plugin, bareConfig)
end

function App.printRegisteredPlugins(self)
    startAudioThreadForPlugin("http", {})
    self.enablePlugin("cspot")
end

app = App.new()

http:handle(http.GET, "/plugins", function(request)
    local pluginNames = {}
    for k, _ in pairs(app.plugins) do
        local value = {
            name = k,
            displayName = app.plugins[k].displayName,
            type = app.plugins[k].type
        }
        table.insert(pluginNames, value)
    end
    -- mock
    table.insert(pluginNames, {
        name = "airplay",
        displayName = "AirPlay integration",
        type = "plugin"
    })

    -- mock
    table.insert(pluginNames, {
        name = "system",
        displayName = "System configuration",
        type = "system"
    })

    -- mock
    table.insert(pluginNames, {
        name = "portaudio",
        displayName = "PortAudio output",
        type = "system"
    })

    -- mock
    table.insert(pluginNames, {
        name = "bluetooth",
        displayName = "Bluetooth sink",
        type = "plugin"
    })

    http:sendJSON(pluginNames, request.connection)
end)

http:handle(http.GET, "/plugins/:name", function(request)
    local plugin = request.urlParams.name
    if (app.plugins[plugin] == nil) then
        http:sendJSON(nil, request.connection)
    else
        for k, v in pairs(app.plugins[plugin].configScheme) do
            if v.value == nil then
                v.value = v.defaultValue
            end
        end
        http:sendJSON(app.plugins[plugin].configScheme, request.connection)
    end
end)

http:handle(http.GET, "/playback", function(request)
    http:sendJSON(app.playbackState, request.connection)
end)

http:handle(http.POST, "/plugins/:name", function(request)
    local plugin = request.urlParams.name
    local body = json.decode(request.body)
    if (app.plugins[plugin] == nil) then
        http:sendJSON(nil, request.connection)
    else
        for k, v in pairs(body) do
            if (app.plugins[plugin].configScheme[k] ~= nil) then
                app.plugins[plugin].configScheme[k].value = v
            end
        end

        http:sendJSON(app.plugins[plugin].configScheme, request.connection)
        app.plugins[plugin]:configurationChanged()
    end
end)
