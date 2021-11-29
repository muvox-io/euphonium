class Plugin
    var name
    var type
    var displayName
    var configSchema
    var exposeWebApp
end

class App
    var eventHandlers
    var plugins
    var playbackState

    def init()
        self.eventHandlers = {
            'handleRouteEvent': def (request)
                http.handleRequest(request)
            end,
            'songChangedEvent': def (playback)
                self.updatePlaybackInfo(playback)
            end
        }
        self.plugins = []
        self.playbackState = {
            'songName': '',
            'artistName': '',
            'sourceName': '',
            'icon': '',
            'albumName': ''
        }
    end

    def handleEvent(eventType, eventData)
        if (self.eventHandlers[eventType])
            self.eventHandlers[eventType](eventData)
        end
    end

    def updatePlaybackInfo(playbackInfo)
        self.playbackState = playbackInfo
    
        http.publishEvent("playback", self.playbackState)
    end

    def registerPlugin(plugin)
        self.plugins.push(plugin)
    end
end

var app = App()

# Native closures
def printRegisteredPlugins()
    print('Registered plugins:')
end

def handleEvent(eventType, eventData)
    app.handleEvent(eventType, eventData)
end

# HTTP Endpoints
http.handle('GET', '/plugins', def (request)
    var result = []
    for plugin : app.plugins
        result.push({
            'name': plugin.name,
            'displayName': plugin.displayName,
            'type': plugin.type
        })

        if plugin.exposeWebApp
            result.push({
                'name': plugin.name,
                'displayName': plugin.displayName,
                'type': 'app'
            })
        end
    end

    http.sendJSON(result, request['connection'], 200)
end)

http.handle('GET', '/plugins/:name', def (request)
    var result = {
        'status': 'error'
    }

    for plugin : app.plugins 
        if plugin.name == request['urlParams']['name']
            result = {
                'displayName': plugin.displayName,
                'configSchema': plugin.configSchema
            }
        end
    end

    http.sendJSON(result, request['connection'], 200)
end)

http.handle('GET', '/playback', def (request)

    http.sendJSON(app.playbackState, request['connection'], 200)
end)

http.handle('POST', '/volume', def (request)
    var body = json.load(request['body'])['volume']
    setVolume(body['volume'])
    http.sendJSON(body, request['connection'], 200)
end)

http.handle('POST', '/eq', def (request)
    var body = json.load(request['body'])
    eqSetBands(real(body['low']), real(body['mid']), real(body['high']))
    http.sendJSON(body, request['connection'], 200)
end)

startAudioThreadForPlugin('http', {})
startAudioThreadForPlugin('webradio', {})
startAudioThreadForPlugin('youtube', {})