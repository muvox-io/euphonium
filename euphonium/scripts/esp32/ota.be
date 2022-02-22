class OTAPlugin : Plugin
    def init()
        self.apply_default_values()
        self.name = "ota"
        self.display_name = "System update"
        self.type = "system"
    end

    def make_form(ctx, state)
        ctx.text_field('name', {
            'label': "Update name",
        })

        ctx.text_field('url', {
            'label': "Update URL",
        })

        ctx.text_field('sha256', {
            'label': "sha256",
        })
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

    request.write_json(result, 200)
    sleep_ms(500)
    ota_reboot_recovery()
end)
