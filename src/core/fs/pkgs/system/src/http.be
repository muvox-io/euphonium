var HTTP_GET = 0
var HTTP_POST = 1

class HTTPRequest
    var conn_id
    def init(conn_id)
        self.conn_id = conn_id
    end

    def member(name)
        return get_native('http', name)
    end

    def url_params()
        return {}
    end

    def query_params()
        return {}
    end

    def json_body()
        return {}
    end

    def write_json(body, status)
        var json_body = json.dump(body ? body : { 'status': 'error' })
        self._write_response(self.conn_id, json_body, "application/json", status ? status : 200)
    end

    def write_text(body, status)
        self._write_response(self.conn_id, body, "text/plain", status ? status : 200)
    end
end

class HTTP
    var registered_handlers_count
    var handlers

    def member(name)
        return get_native('http', name)
    end

    def init()
        self.handlers = []
    end

    def handle_event(req_data)
        var request = HTTPRequest(req_data['conn_id'])
        self.handlers[req_data['handler_id']](request)
    end

    # handle request
    def handle(request_type, path, callback)
        self.handlers.push(callback)
        self._register_handler(request_type, path, self.handlers.size() - 1)
    end

    # publish event
    def emit_event(type, body)
        # _http_publish_event(type, json.dump(body ? body : {}))
    end
end

http = HTTP()

http.handle(HTTP_GET, '/hello_world', def (req)
    req.write_json({ 'status': 'hello_world' })
end)