App = {plugins = {}}
App.__index = App
http = HttpServer.new(2137)

function App.new()
    local self = setmetatable({plugins = {}}, App)
    return self
end

function App.registerPlugin(self, plugin)
    self.plugins[plugin.name] = plugin
end

function App.printRegisteredPlugins(self)
    http:registerHandler(RequestType.GET, "/plugin", function (request)
        local response = HTTPResponse.new()

        response.body = json.encode(self.plugins)
        response.status = 200
        response.contentType = "application/json"
        http:respond(response, request.connection)
    end)

    for k, v in pairs(self.plugins) do
        print(k)
    end
end

app = App.new()