class OTAPlugin : Plugin
    def init()
        self.config_schema = {
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

        self.apply_default_values()
        self.name = "ota"
        self.display_name = "System update"
        self.type = "system"
    end
end

euphonium.register_plugin(OTAPlugin())

http.handle('POST', '/ota', def (request)
    var body = request.json_body()

    var result = {
        'status': 'ok'
    }
    print(body['url'])
    print(body['sha256'])
    persistor.persist('configuration/ota.config.json', json.dump(body))

    http.sendJSON(result, request['connection'], 200)
    sleep_ms(500)
    ota_reboot_recovery()
end)
