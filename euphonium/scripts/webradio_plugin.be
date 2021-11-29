class WebRadioPlugin : Plugin
    def init()
        self.name = "webradio"
        self.displayName = "Web Radio"
        self.type = "plugin"
        self.exposeWebApp = true

        self.configSchema = {
            'radioBrowserApi': {
                'tooltip': 'Radio Browser instance url',
                'type': 'string',
                'defaultValue': 'http://webradio.radiobrowser.com/api/v1/stations/'
            },
        }
    end
end

app.registerPlugin(WebRadioPlugin())

# HTTP Handlers
http.handle('POST', '/webradio', def(request)
    var body = json.load(request['body'])

    app.updatePlaybackInfo({
        'songName': body["stationName"],
        'artistName': 'Internet Radio',
        'sourceName': 'webradio',
        'icon': body['favicon'],
        'albumName': body['codec']
    })
    webradioQueueUrl(body['stationUrl'], (body["codec"] == "AAC" || body["codec"] == "AAC+"))
    http.sendJSON({ 'status': 'playing'}, request['connection'], 200)
end)