Http = {}
Http.__index = Http


function Http.handleIncomingRequest(self, request)
    if self.handlers[request.handlerId] then
        self.handlers[request.handlerId](request)
    end
end

function Http.new()
    local self = setmetatable({}, Http)
    self.handlers = {}
    self.registeredHandlers = 0
    self.GET = RequestType.GET
    self.POST = RequestType.POST
    return self
end

function Http.handle(self, requestType, path, callback)
    self.handlers[self.registeredHandlers] = callback
    httpRegisterHandler(path, requestType, self.registeredHandlers)
    self.registeredHandlers = self.registeredHandlers + 1
end

function Http.sendJSON(self, body, conn, status)
    local response = HTTPResponse.new()
    response.body = json.encode(body or { status = "error" })
    response.status = status or 200
    response.contentType = "application/json"
    httpRespond(response, conn)
end