class JellyfinPlugin : Plugin
    def init()
        # self.configSchema = {
        #     'jellyfin_url': {
        #         'tooltip': 'Jellyfin instance url',
        #         'type': 'string',
        #         'defaultValue': 'https://jellyfin.cf'
        #     },
        #     'jellyfin_username': {
        #         'tooltip': 'Username',
        #         'type': 'string',
        #         'defaultValue': 'test'
        #     },
        #     'jellyfin_password': {
        #         'tooltip': 'Password',
        #         'type': 'string',
        #         'defaultValue': 'dupa12'
        #     },
        # }

        self.apply_default_values()
        self.name = "jellyfin"
        self.theme_color = "#4B0082"
        self.display_name = "Jellyfin"
        self.type = "plugin"
        self.has_web_app = true
    end

    def make_form(ctx, state)
        ctx.create_group('jellyfin', { 'label': 'General' })

        ctx.text_field('jellyfin_url', {
            'label': "Jellyfin instance url",
            'default': "https://jellyfin.cf",
            'group': 'jellyfin'
        })
    end

    def on_event(event, data)
        if event == EVENT_SET_PAUSE
            jellyfin_set_pause(data)
        end
        
        if event == EVENT_CONFIG_UPDATED
            jellyfin_config_updated()
        end
    end
end

euphonium.register_plugin(JellyfinPlugin())

# HTTP Handlers
http.handle('POST', '/jellyfin/playByTrackID', def(request)
    var body = request.json_body()
    jellyfin_playSongByID(body['TrackID'])
    request.write_json({ 'status': 'playing'}, request['connection'], 200)
end)
