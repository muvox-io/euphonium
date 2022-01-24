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

# http.handle('GET', '/plugins/:name', def (request)
#     var result = {
#         'status': 'error'
#     }

#     for plugin : euphonium.plugins
#     if plugin.name == request.get_url_params()['name']
#         for key : plugin.config_schema.keys()
#         if plugin.config_schema[key].find('value') == nil
#             plugin.config_schema[key]['value'] = plugin.config_schema[key]['defaultValue']
#         end
#     end

#     result = {
#         'displayName': plugin.display_name,
#         'themeColor': plugin.theme_color,
#         'configSchema': plugin.config_schema
#     }
# end
# end

# http.sendJSON(result, request['connection'], 200)
# end)

# http.handle('POST', '/plugins/:name', def (request)
#     var result = {
#         'status': 'error'
#     }
#     var bodyObj = json.load(request['body'])

#     var plugin = euphonium.getPluginByName(request['urlParams']['name'])

#     var confSchema = plugin.configSchema

#     for key : bodyObj.keys()
#     confSchema[key]['value'] = bodyObj[key]
# end

# plugin.persistConfig()
# http.sendJSON({ 'configSchema': confSchema, 'displayName': plugin.displayName, 'themeColor': plugin.themeColor }, request['connection'], 200)
# euphonium.sendNotification("info", plugin.name, "Configuration updated")
# plugin.onEvent(EVENT_CONFIG_UPDATED, {})
# end)

# http.handle('GET', '/playback', def (request)
#     http.sendJSON(euphonium.playbackState, request['connection'], 200)
# end)

# http.handle('GET', '/info', def (request)
#     http.sendJSON({
#         'version': core.version(),
#         'networkState': euphonium.networkState
#     }, request['connection'], 200)
# end)

# http.handle('POST', '/volume', def (request)
#     var body = json.load(request['body'])
#     euphonium.applyVolume(int(body['volume']))

#     http.sendJSON(body, request['connection'], 200)
# end)

# http.handle('POST', '/eq', def (request)
#     var body = json.load(request['body'])
#     eqSetBands(real(body['low']), real(body['mid']), real(body['high']))
#     euphonium.playbackState['eq'] = body
#     euphonium.updatePlaybackInfo()
#     http.sendJSON(body, request['connection'], 200)
# end)

# http.handle('POST', '/play', def (request)
#     euphonium.playbackState['status'] = json.load(request['body'])['status']
#     cspot = euphonium.getPluginByName(euphonium.currentPlayer)

#     if euphonium.playbackState['status'] == 'playing'
#         cspot.onEvent(EVENT_SET_PAUSE, false)
#     else
#         cspot.onEvent(EVENT_SET_PAUSE, true)
#     end

#     core.empty_buffers()
#     http.sendJSON({'status': 'ok'}, request['connection'], 200)
# end)
