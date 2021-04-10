App = {plugins = {}}
App.__index = App

function App.new()
    local self = setmetatable({plugins = {}}, App)
    return self
end

function App.registerPlugin(self, plugin)
    self.plugins[plugin.name] = plugin
end

function App.printRegisteredPlugins(self)
    print("corn")

    for k, v in pairs(self.plugins) do
        print(k)
    end

    local c = HttpServer:new(5)
end

app = App.new()