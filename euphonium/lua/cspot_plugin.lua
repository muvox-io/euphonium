CSpotPlugin = {name = "cspot"}
CSpotPlugin.__index = CSpotPlugin

function CSpotPlugin.new()
    local self = setmetatable({name = "cspot"}, CSpotPlugin)
    return self
end

print("UH")
app:registerPlugin(CSpotPlugin.new())