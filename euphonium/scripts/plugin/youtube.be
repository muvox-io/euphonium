YT_STATUS_PLAYING = 1
YT_STATUS_PAUSED = 2
YT_STATUS_LOADING = 3
YT_STATUS_STOPPED = 4

class YouTubePlugin : Plugin
    var ofs
    var playerStatus
    def init()
        self.configSchema = {
        }

        self.ofs = 0
        self.applyDefaultValues()
        self.name = "youtube"
        self.displayName = "YouTube"
        self.playerStatus = YT_STATUS_STOPPED
        self.type = "plugin"
        self.themeColor = "#FF0000"
    end

    def postBind(name, data)
        self.ofs += 1
        var postData = {
            'count': '1',
            'ofs': str(self.ofs),
            'req0__sc': name
        }

        for key : data.keys()
            postData['req0_' + key] = data[key]
        end

        var postString = ""

        for element : postData.keys()
            postString += "&" + str(element) + "=" + str(postData[element])
        end

        print("Got post string " + postString)
        yt_send_bind_data(postString)
    end

    def onEvent(ev, data)
        print("Got event")
        var eventBody = json.load(data['body'])
        var eventName = eventBody[1][0]

        print("Received LEAN event " + eventName)
        if eventName == 'getNowPlaying'
            self.postBind('nowPlaying', {})
        end

        if eventName == 'setPlaylist'
            print("Setting playlist")

            eventData = eventBody[1][1]
            app.updateSong({
            'songName': 'YouTube',
            'artistName': 'YouTube',
            'sourceName': 'youtube',
            'icon': 'https://upload.wikimedia.org/wikipedia/commons/thumb/7/75/YouTube_social_white_squircle_%282017%29.svg/1024px-YouTube_social_white_squircle_%282017%29.svg.png',
            'albumName': '-'
            })
            youtubeQueueUrl(eventData['videoId'])
            self.postBind("nowPlaying", { 'videoId': eventData['videoId'], 'currentTime': 0, 'listId': eventData['listId'], 'currentIndex': '1', 'state': '3'})
            self.postBind("onStateChange", { 'currentTime': '0', 'state': '1', 'cpn': 'foo' })
            print(eventData)
        end

        if eventName == 'getVolume'
            self.postBind('onVolumeChanged', { 'volume': '5', 'muted': 'false' })
        end

        if eventName == 'remoteConnected'
            eventData = eventBody[1][1]
            print(eventData)
        end
        
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
