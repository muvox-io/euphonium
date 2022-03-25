class WebRadioPlugin : Plugin
    def init()
        self.apply_default_values()
        self.name = "webradio"
        self.theme_color = "#d2c464"
        self.display_name = "Web Radio"
        self.type = "plugin"
        self.has_web_app = true
    end

    def make_form(ctx, state)

        ctx.create_group('driver', { 'label': 'Driver' })
        ctx.create_group('i2s', { 'label': 'I2S GPIO' })
        ctx.create_group('i2c', { 'label': 'I2C GPIO' })

        ctx.select_field('dac', {
            'label': "Select driver",
            'default': "I2S",
            'group': 'driver',
            'values': ['TAS5711', 'MA12070P', 'I2S'],
            'type': 'number'
        })

        ctx.number_field('bck', {
            'label': "BCK",
            'default': "0",
            'group': 'i2s',
        })

        ctx.number_field('ws', {
            'label': "WS",
            'default': "0",
            'group': 'i2s',
        })

        ctx.number_field('data', {
            'label': "DATA",
            'default': "0",
            'group': 'i2s',
        })

        ctx.number_field('mclk', {
            'label': "MCLK",
            'default': "0",
            'group': 'i2s',
        })

        ctx.number_field('sda', {
            'label': "SDA",
            'default': "0",
            'group': 'i2c',
        })

        ctx.number_field('scl', {
            'label': "SCL",
            'default': "0",
            'group': 'i2c',
        })

    end


    def on_event(event, data)
        if event == EVENT_SET_PAUSE
            webradio_set_pause(data)
        end
    end
end

euphonium.register_plugin(WebRadioPlugin())

# HTTP Handlers
http.handle('POST', '/webradio', def(request)
    var body = request.json_body()

    euphonium.update_song({
        'songName': body["stationName"],
        'artistName': 'Internet Radio',
        'sourceName': 'webradio',
        'icon': body['favicon'],
        'albumName': body['codec']
    })
    webradio_queue_url(body['stationUrl'], (body["codec"] == "AAC" || body["codec"] == "AAC+"))
    euphonium.set_status('playing')
    request.write_json({ 'status': 'playing'}, 200)
end)
