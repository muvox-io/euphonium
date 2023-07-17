
import debug

var HTTP_GET = 0
var HTTP_POST = 1

class HTTPRequest
    var conn_id
    var did_write
    def init(conn_id)
        self.conn_id = conn_id
        self.did_write = false
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
        return json.load(json_body)
    end

    def extract_tar(destPath)
        self._extract_tar(self.conn_id, destPath)
    end

    def formencoded_body()
        var formencoded_body = self._read_body(self.conn_id)
        var body = {}
        var pairs = string.split(formencoded_body, '&')
        for pair : pairs
            var key_value = string.split(pair, '=')
            body[key_value[0]] = key_value[1]
        end
        return body
    end

    def write_json(body, status)
        var json_body = json.dump(body ? body : { 'status': 'error' })
        self._write_response(self.conn_id, json_body, "application/json", status ? status : 200)
        self.did_write = true
    end

    def write_text(body, status)
        self._write_response(self.conn_id, body, "text/plain", status ? status : 200)
        self.did_write = true
    end

    def write_tar(sourcePath, filename)
        self._write_tar_response(self.conn_id, sourcePath, filename)
        self.did_write = true
    end

    def write_raw(body, status, content_type)
        self._write_response(self.conn_id, body, content_type, status ? status : 200)
        self.did_write = true
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

    def register_mdns(name, type, proto, txt)
        self._register_mdns(name, type, proto, txt)
    end

    def handle_event(req_data)
        var request = HTTPRequest(req_data['conn_id'])
        try
            self.handlers[req_data['handler_id']](request)
        except .. as e,m
            print('exception in http handler: ' + e + ': ' + m)
            debug.traceback()
            # respond with error if an exception was thrown before the request was written
            if !request.did_write
                request.write_json({
                    'status': 'error',
                    'error': e + ': ' + m,
                }, 500)
            end
        end
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
