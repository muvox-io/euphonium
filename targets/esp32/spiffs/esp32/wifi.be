class WiFiPlugin : Plugin
    var wifiState
    def init()
        self.configSchema = {
            'ssid': {
                'tooltip': 'WiFi SSID',
                'type': 'string',
                'value': '',
                'defaultValue': ""
            },
            'password': {
                'tooltip': 'WiFi password',
                'type': 'string',
                'defaultValue': ""
            },
        }
        self.wifiState = {}

        self.applyDefaultValues()
        self.name = "wifi"
        self.displayName = "WiFi"
        self.type = "init_handler"

        app.registerHandler('wifiStateChanged', def (state)
            print(state)
            if state['state'] == 'connected'
                print("Connected to wifi")
                self.wifiState['state'] = 'connected'
                self.wifiState['ipAddress'] = state['ipAddress']
                http.publishEvent('wifi_state', self.wifiState)
                app.initRequiredPlugins()
            end
            if state['state'] == 'ap_ready'
                app.initHTTP()
            end

            if state['state'] == 'no_ap'
                print("No wifi access point found")
                #wifi_start_ap("Euphonium", "euphonium")
            end

            if state['state'] == 'connecting'
                self.wifiState['state'] = 'connecting'
                http.publishEvent('wifi_state', self.wifiState)
            end

            if state['state'] == 'scan_done'
                self.wifiState = {
                    'status': 'scanning',
                    'ssids': []
                }

                for ssid : state['networks'].keys()
                    self.wifiState['ssids'].push({
                        'ssid': ssid,
                        'open': state['networks'][ssid]['open']
                    })
                end

                http.publishEvent('wifi_state', self.wifiState)
                wifi_start_scan()
            end
        end)
    end

    def onSystemInit()
        print('on system init called')
        wifi_init()
        if self.configValue('ssid') != ""
            wifi_connect(self.configValue('ssid'), self.configValue('password'), false)
        else
            # start access point for config
            wifi_start_ap("Euphonium", "euphonium")
        end
    end
end

var wifi = WiFiPlugin()

http.handle('GET', '/wifi/wifi_scan', def (request)
    var result = {
        'status': 'scanning'
    }

    http.sendJSON(result, request['connection'], 200)
    wifi_start_scan()
end)


http.handle('GET', '/wifi/status', def (request)
    http.sendJSON(wifi.wifiState, request['connection'], 200)
end)

http.handle('POST', '/wifi/connect', def (request)
    var body = json.load(request['body'])

    var result = {
        'status': 'connecting'
    }
    print(body['ssid'])
    print(body['password'])

    wifi.configSchema['ssid']['value'] = body['ssid']
    wifi.configSchema['password']['value'] = body['password']

    wifi_connect(body['ssid'], body['password'], true)
end)

app.registerPlugin(wifi)