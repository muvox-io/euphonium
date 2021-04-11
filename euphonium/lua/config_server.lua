ConfigServer = {plugins = {}}
ConfigServer.__index = App
http = HttpServer.new(2137)

function ConfigServer.new()
    local self = setmetatable({plugins = {}}, ConfigServer)
    return self
end

function ConfigServer.registerConfigField(self, plugin)
    self.plugins[plugin.name] = plugin
end

configServer = ConfigServer.new()