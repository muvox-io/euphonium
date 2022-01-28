# HTTP Endpoints
http.handle('GET', '/plugins', def (request)
    var result = []
    for plugin : euphonium.plugins
        result.push({
            'name': plugin.name,
            'displayName': plugin.display_name,
            'type': plugin.type
        })

        if plugin.has_web_app
            result.push({
                'name': plugin.name,
                'displayName': plugin.display_name,
                'type': 'app'
            })
        end
    end

    request.write_json(result, 200)
end)

http.handle('GET', '/plugins/:name', def (request)
    var result = {
        'status': 'error'
    }

    for plugin : euphonium.plugins
        if plugin.name == request.url_params()['name']
            for key : plugin.config_schema.keys()
                if plugin.config_schema[key].find('value') == nil
                    plugin.config_schema[key]['value'] = plugin.config_schema[key]['defaultValue']
                end
            end

            result = {
                'displayName': plugin.display_name,
                'themeColor': plugin.theme_color,
                'configSchema': plugin.config_schema
            }
        end
    end
    request.write_json(result, 200)
end)

http.handle('POST', '/plugins/:name', def (request)
    var result = {
        'status': 'error'
    }
    var body = request.json_body()

    var plugin = euphonium.get_plugin(request.url_params()['name'])

    var conf_schema = plugin.config_schema

    for key : body.keys()
        conf_schema[key]['value'] = body[key]
    end

    plugin.persist_config()
    request.write_json(
        {
            'configSchema': conf_schema, 
            'displayName': plugin.display_name,
            'themeColor': plugin.theme_color
        }, 200)
    euphonium.send_notification("info", plugin.name, "Configuration updated")
    plugin.on_event(EVENT_CONFIG_UPDATED, {})
end)

http.handle('GET', '/system', def (request)
    request.write_json({
        'version': core.version(),
        'networkState': euphonium.network_state
    }, 200)
end)

http.handle('GET', '/playback', def (request)
    request.write_json(euphonium.playback_state, 200)
end)

http.handle('POST', '/playback/volume', def (request)
    var body = request.json_body()
    euphonium.apply_volume(int(body['volume']))

    request.write_json(euphonium.playback_state, 200)
end)

http.handle('POST', '/playback/eq', def (request)
    var body = request.json_body()
    playback.set_eq(real(body['low']), real(body['mid']), real(body['high']))
    euphonium.playback_state['eq'] = body
    euphonium.update_playback()
    request.write_json(euphonium.playback_state, 200)
end)

http.handle('POST', '/playback/status', def (request)
    euphonium.playback_state['status'] = request.json_body()['status']
    plugin = euphonium.get_plugin(euphonium.current_source)

    if euphonium.playback_state['status'] == 'playing'
        plugin.on_event(EVENT_SET_PAUSE, false)
    else
        plugin.on_event(EVENT_SET_PAUSE, true)
    end

    # reset buffers
    playback.empty_buffers()
    request.write_json(euphonium.playback_state, 200)
end)
