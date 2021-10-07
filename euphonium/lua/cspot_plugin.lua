CSpotPlugin = {name = "cspot"}
CSpotPlugin.__index = CSpotPlugin

function CSpotPlugin.new()
    local self = setmetatable({name = "cspot", displayName = "Spotify integration", type="plugin"}, CSpotPlugin)

    -- Plugin configuration schema
    self.configScheme = {
        receiverName = {
            tooltip = "Speaker's name",
            type = "string",
            defaultValue = "CSpot (Euphonium)"
        },
        audioBitrate = {
            tooltip = "Audio bitrate",
            type = "stringList",
            listValues = {"96", "160", "320"},
            defaultValue = "160"
        }
    }
    return self
end

function CSpotPlugin.configurationChanged(self)
    print("CSpot: Configuration changed")
end

app:registerPlugin(CSpotPlugin.new())