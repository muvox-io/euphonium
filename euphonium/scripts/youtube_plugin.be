class YouTubePlugin : Plugin
    def init()
        self.configSchema = {
        }

        self.applyDefaultValues()
        self.name = "youtube"
        self.displayName = "YouTube"
        self.type = "plugin"
    end
end

app.registerPlugin(YouTubePlugin())

# HTTP Handlers
http.handle('POST', '/youtube', def(request)
    var body = json.load(request['body'])
    app.updateSong({
        'songName': 'YouTube',
        'artistName': 'YouTube',
        'sourceName': 'youtube',
        'icon': 'https://upload.wikimedia.org/wikipedia/commons/thumb/7/75/YouTube_social_white_squircle_%282017%29.svg/1024px-YouTube_social_white_squircle_%282017%29.svg.png',
        'albumName': '-'
    })
    print(body['videoUrl'])
    youtubeQueueUrl(body['videoUrl'])
    http.sendJSON({ 'status': 'playing'}, request['connection'], 200)
end)
