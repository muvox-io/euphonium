class YouTubePlugin : Plugin
    def init()
        self.name = "youtube"
        self.displayName = "YouTube"
        self.type = "plugin"
        self.exposeWebApp = false

        self.configSchema = {
        }
    end
end

app.registerPlugin(YouTubePlugin())

# HTTP Handlers
http.handle('POST', '/youtube', def(request)
    var body = json.load(request['body'])

    app.updatePlaybackInfo({
        'songName': 'YouTube',
        'artistName': 'YouTube',
        'sourceName': 'youtube',
        'icon': '?',
        'albumName': '?'
    })
    youtubeQueueUrl(body['stationUrl'])
    http.sendJSON({ 'status': 'playing'}, request['connection'], 200)
end)