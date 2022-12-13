class HTTPRequest
    var fd
    var req_object
    def init(request)
        self.fd = request['connection']
        self.req_object = request
    end

    def url_params()
        return self.req_object['urlParams']
    end

    def query_params()
        return self.req_object['queryParams']
    end

    def json_body()
        return json.load(self.req_object['body'])
    end

    def write_json(body, status)
        var json_body = json.dump(body ? body : { 'status': 'error' })
        _http_respond(
            self.fd, 
            status ? status : 200, 
            json_body, 
            "application/json")
    end

    def write_text(body, status)
        _http_respond(
            self.fd,
            status ? status : 200,
            body,
            "text/plain")
    end
end

class HTTP
    var registered_handlers_count
    var handlers

    def init()
        self.handlers = []
    end

    def handle_event(req_data)
        var request = HTTPRequest(req_data)
        self.handlers[req_data['handlerId']](request)
    end

    # handle request
    def handle(request_type, path, callback)
        self.handlers.push(callback)
        _http_register_handler(path, request_type, self.handlers.size() - 1)
    end

    # publish event
    def emit_event(type, body)
        _http_publish_event(type, json.dump(body ? body : {}))
    end
end

http = HTTP()