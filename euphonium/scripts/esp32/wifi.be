class WiFiPlugin : Plugin
    var wifi_state
    def init()
        self.wifi_state = {}

        self.apply_default_values()
        self.name = "wifi"
        self.display_name = "WiFi"
        self.type = "init_handler"
        euphonium.register_handler('wifiStateChanged', def (state)
            if state['state'] == 'connected'
                print("Connected to wifi")
                self.wifi_state['state'] = 'connected'
                self.wifi_state['ipAddress'] = state['ipAddress']
                print(self.config_schema)
                self.persist_config()
                http.emit_event('wifi_state', self.wifi_state)
                euphonium.init_required_plugins()
            end

            if state['state'] == 'ap_ready'
                euphonium.init_http()
            end

            if state['state'] == 'no_ap'
                self.wifi_state['state'] = 'error'
                http.emit_event('wifi_state', self.wifi_state)
                print("No wifi access point found")
                wifi.start_ap("Euphonium", "euphonium")
                self.wifi_state['state'] = 'scanning';
                #wifi_start_ap("Euphonium", "euphonium")
            end

            if state['state'] == 'connecting'
                self.wifi_state['state'] = 'connecting'
                http.emit_event('wifi_state', self.wifi_state)
            end

            if state['state'] == 'scan_done'
                self.wifi_state['ssids'] = [] 

                for ssid : state['networks'].keys()
                    self.wifi_state['ssids'].push({
                        'ssid': ssid,
                        'open': state['networks'][ssid]['open']
                    })
                end
                self.wifi_state['state'] = 'scanning'

                http.emit_event('wifi_state', self.wifi_state)
                wifi.start_scan()
            end
        end)
    end
    def on_event(event, data)
        if event == EVENT_SYSTEM_INIT
            print("Calling WiFi init")
            wifi.init_stack()
            print(self.config_schema)
            if self.config_value('ssid') != ""
                wifi.connect(self.config_value('ssid'), self.config_value('password'), false)
                euphonium.get_plugin('general').set_hostname()
            else
                # start access point for config
                wifi.start_ap("Euphonium", "euphonium")
            end
        end
    end

    def make_form(ctx, state)
        ctx.text_field('ssid', {
            'label': "WiFi SSID",
            'default': ''
        })

        ctx.text_field('password', {
            'label': "WiFi password",
            'default': ''
        })
    end
end

var wifi_plugin = WiFiPlugin()

http.handle('GET', '/wifi/wifi_scan', def (request)
    var result = {
        'status': 'scanning'
    }

    request.write_json(result, 200)
    wifi.start_scan()
end)


http.handle('GET', '/wifi/status', def (request)
    request.write_json(wifi_plugin.wifi_state, 200)
end)

http.handle('POST', '/wifi/connect', def (request)
    var body = request.json_body()

    var result = {
        'status': 'connecting'
    }
    print(body['ssid'])
    print(body['password'])

    wifi_plugin.state['ssid'] = body['ssid']
    wifi_plugin.state['password'] = body['password']

    wifi_plugin.wifi_state['state'] = 'connecting'
    http.emit_event('wifi_state', wifi_plugin.wifi_state)
    wifi.connect(body['ssid'], body['password'], true)
    http.emit_event(result, 200)
end)

euphonium.register_plugin(wifi_plugin)
