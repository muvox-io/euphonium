# Load essential modules
core.load("internal/http.be")
core.load("internal/plugin.be")
core.load("internal/euphonium.be")
core.load("internal/api.be")

# Load plugins
core.load("plugin/cspot.be")
core.load("plugin/webradio.be")

# Load ESP32 modules
if core.platform() == "esp32"
    core.load("esp32/wifi.be")
    core.load("esp32/bluetooth.be")
    core.load("esp32/i2s.be")
    core.load("esp32/dac_plugin.be")
    core.load("esp32/dacs/es8388.be")
    core.load("esp32/dacs/ac101_driver.be")
end

# load user configuration
core.load("extensions.be")