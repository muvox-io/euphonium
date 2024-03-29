import math

# HTTP Endpoints
http.handle(HTTP_GET, '/plugins', def (request)
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

http.handle(HTTP_GET, '/plugins/:name', def (request)
    for plugin : euphonium.plugins
        if plugin.name == request.route_params()['name']
            var ctx = FormContext()
            var state = json.load(json.dump(plugin.state)) # deep copy

            plugin.make_form(ctx, state)
            request.write_json({
                'displayName': plugin.display_name,
                'themeColor': plugin.theme_color,
                'configSchema': ctx.children,
                'state': plugin.state,
                'refresh_interval': ctx.refresh_interval
            }, 200)
            return
        end
    end
    request.write_json({
        "status": "error",
        "error": "Plugin not found"
    }, 404)
end)

http.handle(HTTP_POST, '/plugins/:name', def (request)
    var result = {
        'status': 'error'
    }
    var body = request.json_body()
    var plugin = euphonium.get_plugin(request.route_params()['name'])
    var isDraft = body['isPreview']
    var state = json.load(json.dump(plugin.state)) # deep copy
    var events = body['events']
    for key : body['state'].keys()
        state[key] = body['state'][key]
    end

    var ctx = FormContext(events)
    plugin.make_form(ctx, state)

    if ctx.redraw_requested # some plugins mutate the state they depend on, so a second pass is required
        ctx = FormContext() # reset the context, but don't pass the events
        plugin.make_form(ctx, state)
    end

    result = { 
        'displayName': plugin.display_name,
        'themeColor': plugin.theme_color,
        'configSchema': ctx.children,
        'state': state,
        'refresh_interval': ctx.refresh_interval
    }
    request.write_json(result, 200)
end)

# Returns a list of all plugins which request to show a global modal
# The client then needs to fetch the plugin's config form and display the modals in it
http.handle(HTTP_GET, '/global-modals', def (request)
    result = []
    for plugin : euphonium.plugins
        var ctx = FormContext()
        plugin.make_form(ctx, plugin.state)
        if ctx.has_global_modal()
            result.push(plugin.name)
        end
    end
    request.write_json(result, 200)
end)


http.handle(HTTP_GET, '/system', def (request)
    request.write_json({
        'version': core.version(),
        'networkState': euphonium.network_state
    }, 200)
end)

http.handle(HTTP_GET, '/playback', def (request)
    request.write_json(playback_state.get_state(), 200)
end)

http.handle(HTTP_GET, '/playback/recent', def (request)
    request.write_json(playback_state.recently_played, 200)
end)

http.handle(HTTP_POST, '/playback/volume', def (request)
    var body = request.json_body()
    euphonium.apply_volume(int(body['volume']))

    if body.find('persist') != nil && body['persist']
        general_settings.set_volume(int(body['volume']))
    end

    request.write_json(playback_state.get_state(), 200)
end)

http.handle(HTTP_POST, '/playback/eq', def (request)
    var body = request.json_body()
    playback.set_eq(real(body['low']), real(body['mid']), real(body['high']))
    euphonium.playback_state['eq'] = body
    euphonium.update_playback()
    
    # save to file if persist is requrested
    if body.find('persist') != nil && body['persist']
        euphonium.persist_playback_state()
    end

    request.write_json(euphonium.playback_state, 200)
end)


http.handle(HTTP_POST, '/playback/dsp', def (request)
    var body = request.json_body()

    if body.find("transforms") != nil
      playback.configure_dsp(json.dump(body))
    end

    request.write_json(playback_state.get_state(), 200)
end)

http.handle(HTTP_POST, '/playback/status', def (request)
    var body = request.json_body()
    playback_state.notify_state(body['state'])

    request.write_json(playback_state.get_state(), 200)
end)

http.handle(HTTP_POST, '/playback/context', def (request)
    var body = request.json_body()
    if body.find('uri') != nil
        core.query_context_uri(body['uri'])
    end
    request.write_json(playback_state.get_state(), 200)
end)


http.handle(HTTP_GET, '/config_backup', def (request)
    var unix_time = int(math.floor(core.get_time_ms() / 1000))
    request.write_tar("/cfg", "euphonium-backup-" + str(unix_time) + ".tar")
end)

http.handle(HTTP_POST, '/config_backup', def (request)
    
    request.extract_tar("/cfg")
    request.write_json({
        'status': 'ok'
    }, 200)
end)

# Register for MDNS
http.register_mdns(general_settings.get_name(), "_http", "_tcp", { "CPath": "/" })
