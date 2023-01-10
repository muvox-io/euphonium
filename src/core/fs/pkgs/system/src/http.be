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

    def route_params()
        return self._read_route_params(self.conn_id)
    end

    def query_params()
        return {}
    end

    def json_body()
        var json_body = self._read_body(self.conn_id)
        print(json_body)
        return json.load(json_body)
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

        events.register_native('http_request', def (event) 
            self.handle_event(event)
        end)
    end

    def handle_event(req_data)
        var request = HTTPRequest(req_data['conn_id'])
        self.handlers[req_data['handler_id']](request)
    end

    # handle request
    def handle(request_type, path, callback)
        self.handlers.push(callback)
        self._register_handler(int(request_type), path, int(self.handlers.size() - 1))
    end

    # publish event
    def emit_event(type, body)
        var json_body = json.dump({ 'type': type, 'data': body })
        self._broadcast_websocket(json_body)
    end
end

http = HTTP()
