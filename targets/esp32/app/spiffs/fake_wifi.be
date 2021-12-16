class FakeWiFiPlugin : Plugin
    def init()
        self.configSchema = {
        }

        self.applyDefaultValues()
        self.name = "wifi"
        self.displayName = "WiFi"
        self.type = "system_hidden"
    end
end

app.registerPlugin(FakeWiFiPlugin())

http.handle('GET', '/wifi/wifi_scan', def (request)
    var result = {
        'status': 'scanning'
    }

    http.sendJSON(result, request['connection'], 200)
    sleep_ms(3000)
    http.publishEvent('wifi_state', {
        'status': 'scanning',
        'ssids': [
            { 'name': 'SYRION7624', 'open': false },
            { 'name': 'Crab_Free_WiFi_2.4G', 'open': false },
            { 'name': 'Test siec', 'open': true }
        ]
    })
end)

http.handle('POST', '/wifi/connect', def (request)
    var body = json.load(request['body'])

    var result = {
        'status': 'connecting'
    }
    print(body['ssid'])
    print(body['password'])

    http.sendJSON(result, request['connection'], 200)
    http.publishEvent('wifi_state', {
        'status': 'connecting',
        'ssids': [
            { 'name': 'SYRION7624', 'open': false },
            { 'name': 'Crab_Free_WiFi_2.4G', 'open': false },
            { 'name': 'Test siec', 'open': true }
        ]
    })

    sleep_ms(3000)
    if body['password'] == 'DUPA123'
        http.publishEvent('wifi_state', {
            'status': 'connected',
            'ssids': [
                { 'name': 'SYRION7624', 'open': false },
                { 'name': 'Crab_Free_WiFi_2.4G', 'open': false },
                { 'name': 'Test siec', 'open': true }
            ]
        })
    else
        http.publishEvent('wifi_state', {
            'status': 'error',
            'ssids': [
                { 'name': 'SYRION7624', 'open': false },
                { 'name': 'Crab_Free_WiFi_2.4G', 'open': false },
                { 'name': 'Test siec', 'open': true }
            ]
        })
    end
end)