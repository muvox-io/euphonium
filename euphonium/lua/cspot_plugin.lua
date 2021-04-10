CSpotPlugin = {name = "cspot"}
CSpotPlugin.__index = CSpotPlugin

function CSpotPlugin.new()
    local self = setmetatable({name = "cspot"}, CSpotPlugin)
    return self
end

print("The doopsko has arrived")
app:registerPlugin(CSpotPlugin.new())
app:printRegisteredPlugins()

http:registerHandler(RequestType.GET, "/horse", function (request)
    local response = HTTPResponse.new()
    response.body = "<img src=\"https://cdn.pixabay.com/photo/2018/08/17/01/14/horse-3611921__340.jpg\"/>"
    response.status = 200
    response.contentType = "text/html"
    print(request["connection"])
    http:respond(response, request.connection)
end)

http:registerHandler(RequestType.GET, "/")

http:listen()