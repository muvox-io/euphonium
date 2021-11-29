class HTTP
    var registeredHandlerCount
    var handlers

    def init()
        self.handlers = []
    end

    def handleRequest(request)
        print(request)
        self.handlers[request['handlerId']](request)
    end

    # handle request
    def handle(requestType, path, callback)
        self.handlers.push(callback)
        httpRegisterHandler(path, requestType, self.handlers.size() - 1)
    end

    # publish event
    def publishEvent(type, body)
        httpPublishEvent(type, json.dump(body ? body : {}))
    end

    # write bare json response to given connection
    def sendJSON(body, conn, status)
        httpRespond(
            conn, 
            status ? status : 200, 
            json.dump(body ? body : { 'status': 'error' }), 
            "application/json")
    end
end

http = HTTP()