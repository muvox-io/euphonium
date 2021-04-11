Http = {}
Http.__index = Http

function Http.new(port)
    local self = setmetatable({}, Http)
    self.server = HttpServer.new(port)
    self.GET = RequestType.GET
    self.POST = RequestType.POST
    return self
end

function Http.handle(self, requestType, path, callback)
    self.server:registerHandler(requestType, path, callback)
end

function Http.listen(self)
    self.server:listen()
end

function Http.sendJSON(self, body, conn, status)
    local response = HTTPResponse.new()
    response.body = json.encode(body or { status = "error" })
    response.status = status or 200
    response.contentType = "application/json"
    self.server:respond(response, conn)
end