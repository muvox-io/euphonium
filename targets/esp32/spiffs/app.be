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
            'songChangedEvent': def (song)
                self.updateSong(song)
            end
        }
        self.plugins = []
        self.playbackState = {
            'song': {
                'songName': 'Queue empty',
                'artistName': '--',
                'sourceName': '--',
                'icon': '',
                'albumName': '--'
            },
            'eq': {
                'low': 0,
                'mid': 0,
                'high': 0
            },
            'volume': 50,
            'status': 'paused'
        }
    end

    def handleEvent(eventType, eventData)
        if (self.eventHandlers[eventType])
            self.eventHandlers[eventType](eventData)
        end
    end

    def updateSong(playbackInfo)
        self.playbackState['song'] = playbackInfo
        self.updatePlaybackInfo()
    end

    def setStatus(playbackStatus)
        self.playbackState['status'] = playbackStatus
        self.updatePlaybackInfo()
    end

    def updatePlaybackInfo()
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
    var body = json.load(request['body'])
    setVolume(body['volume'])
    app.playbackState['volume'] = body['volume']
    app.updatePlaybackInfo()
    http.sendJSON(body, request['connection'], 200)
end)

http.handle('POST', '/eq', def (request)
    var body = json.load(request['body'])
    eqSetBands(real(body['low']), real(body['mid']), real(body['high']))
    app.playbackState['eq'] = body
    app.updatePlaybackInfo()
    http.sendJSON(body, request['connection'], 200)
end)

startAudioThreadForPlugin('http', {})
startAudioThreadForPlugin('webradio', {})
startAudioThreadForPlugin('youtube', {})
