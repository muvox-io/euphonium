class OTAPlugin : Plugin
    def init()
        self.configSchema = {
            'name': {
                'type': 'hidden',
                'defaultValue': ""
            },
            'url': {
                'type': 'hidden',
                'defaultValue': ""
            },
            'sha256': {
                'type': 'hidden',
                'defaultValue': ""
            },
        }

        self.applyDefaultValues()
        self.name = "ota"
        self.displayName = "System update"
        self.type = "system"
    end
end

app.registerPlugin(OTAPlugin())

http.handle('POST', '/set_ota', def (request)
    var body = json.load(request['body'])

    var result = {
        'status': 'ok'
    }
    print(body['url'])
    print(body['sha256'])
    conf_persist('ota.config.json', json.dump(body))

    http.sendJSON(result, request['connection'], 200)
    sleep_ms(500)
    ota_reboot_recovery()
end)
