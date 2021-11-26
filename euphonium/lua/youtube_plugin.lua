YouTubePlugin = {name = "youtube"}
YouTubePlugin.__index = YouTubePlugin

function YouTubePlugin.new()
    local self = setmetatable({name = "youtube", displayName = "YouTube", type="plugin"}, YouTubePlugin)

    -- Plugin configuration schema
    self.configScheme = {
        -- stationUrl = {
        --     tooltip = "RadioBrowser instance",
        --     type = "string",
        --     defaultValue = "https://de1.api.radio-browser.info/"
        -- },
    }
    return self
end

function YouTubePlugin.configurationChanged(self)
    logInfo("Configuration changed")
    startAudioThreadForPlugin("youtube", getBareConfig(self.configScheme))
end

http:handle(http.POST, "/youtube", function(request)
    local body = json.decode(request.body)
    youtubeQueueUrl(body.videoUrl)
    http:sendJSON({ status = "playing" }, request.connection)

    -- update playback status
    -- local song = {
    --     songName = body["stationName"],
    --     artistName = "Internet Radio",
    --     sourceName = "webradio",
    --     icon = body["favicon"],
    --     albumName = body["codec"]
    -- }
    -- handleSongChangedEvent(song)
end)

app:registerPlugin(YouTubePlugin.new())