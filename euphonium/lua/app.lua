App = {
    plugins = {}
}
App.__index = App
http = Http.new(2137)

function App.new()
    local self = setmetatable({
        plugins = {}
    }, App)
    return self
end

function App.registerPlugin(self, plugin)
    self.plugins[plugin.name] = plugin
end

function App.printRegisteredPlugins(self)
    startAudioThreadForPlugin("cspot")
    http:listen()
end

app = App.new()

http:handle(http.GET, "/plugins", function(request)
    local pluginNames = {}
    for k, _ in pairs(app.plugins) do
        table.insert(pluginNames, k)
    end

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

        app.plugins[plugin]:configurationChanged()
        http:sendJSON(app.plugins[plugin].configScheme, request.connection)
    end
end)
