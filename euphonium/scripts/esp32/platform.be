
# HTTP Handlers
http.handle('GET', '/report_ram', def (request)
    esp32_report_ram()
    http.sendJSON({}, request['connection'], 200)
end)