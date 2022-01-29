# Scripting language API

Euphonium contains a berry-lang scripting language that can be used to tweak the system for your needs. The following page documents the internal API used in all scripts. Feel free to check `euphonium/scripts` to see how this is used internally.

## `globals`

Global utilities

### Commands

| Command    | Signature                                                                                 | Supported platforms |
|:-----------|-------------------------------------------------------------------------------------------|---------------------|
| `sleep_ms` | `(milliseconds: number) -> void`. <br/>Pauses execution for given amount of milliseconds. | All                 |
| `bin`      | `(binary: string) -> int`. <br/>Converts binary string to int.                            | All                 |


## `core`

Manages euphonium's core functionality, mostly shared utils. 

**Implemented by `Core.cpp`**

### Commands

| Command              | Signature                                                                                                              | Supported platforms |
|:---------------------|------------------------------------------------------------------------------------------------------------------------|---------------------|
 | `core.start_plugin`  | `(pluginName: string, pluginConfig: map) -> void`<br/>Starts given plugin's audio thread with following configuration. | All                 |
| `core.platform`      | `() -> string`.<br/>Returns platform on which euphonium is currently running. Result being either `esp32` or `desktop` | All                 |
| `core.version`       | `() -> string`.<br/>Returns current version of the system. Example result: `0.0.14`                                    | All                 |

## `http`
Allows for registering endpoints on the internal HTTP server.

**Implemented by `http.be` and `HTTPModule.cpp`**

### Commands

| Command           | Signature                                                                                                                                                                                                                                | Supported platforms |
|:------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| `http.handle`     | `(method: string, path: string, response_handler: [(request) -> void]) -> void`<br/>Register a new HTTP endpoint under given `path`. Response handler is a method that takes `HTTPRequest` as a parameter. See examples below for usage. | All                 |
| `http.emit_event` | `(type: string, body: map) -> void`.<br/>Broadcasts a server-side event to all connected devices. `body` will be serialized into json string.                                                                                            | All                 |

### Object `HTTPRequest`


| Command        | Signature                                                                                                                                                                                       | Supported platforms |
|:---------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| `write_json`   | `(body: map, status: int) -> void`.<br/>Writes a response json to given connection. Body is passed as a map of data to be serialized into json. Status is the HTTP status code of the response. | All                 |
| `write_text`   | `(body: string, status: int) -> void`.<br/>Writes the response as `text/plain`.                                                                                                                 | All                 |
| `json_body`    | `() -> map`<br/>Parses given request's body string into json and returns it as map                                                                                                              | All                 |
| `query_params` | `() -> map`<br/>Returns map of parsed query parameters of the given request.                                                                                                                    | All                 |
| `url_params`   | `() -> map`<br/>Returns map of parsed url parameters of the given request.                                                                                                                      | All                 |


### Example

!!! example "HTTP server usage"

    Handle simple GET and return "Hello, world!"

    ```python
    http.handle('GET', '/hello_world', def (request)
        request.write_text("Hello world!", 200)
    end)
    ```
    Handle POST with json body, return a json response

    ```python
    http.handle('POST', '/create_cat', def (request)
        if request.json_body() == nil
            http.write_text("No body", request['connection'], 400)
        else
           # Parse json body
           var parsed_body = request.json_body()

           # Create response
           var response = { 'name': parsed_body['name'], 'age': 3 }
           request.write_json(response, 200)
        end
    end)
    ```

## `playback`

Manages playback state of the system.

### Commands

| Command                  | Signature                                                                                                                                                                        | Supported platforms |
|:-------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| `playback.set_eq`        | `(low: int, mid: int, high: int) -> void`.<br/>If `EqualizerProcessor` is enabled, this changes the eq's settings. 1 point on the scale means 3 db. Defaults to 0, 0, 0 (no eq). | All                 |
| `playback.set_pause`     | `(paused: boolean) -> void`.<br/>Pauses the playback state. This also triggers a pause event.                                                                                    | All                 |
| `playback.empty_buffers` | `() -> void`.<br/>Empties internal audio buffers of the system. Call this during playback changes / stop pause.                                                                  | All                 |
| `playback.soft_volume`   | `(volume: int) -> void`<br/>Changes the system's software volume. Volume is between `0` and `100`.                                                                               | All                 |

## `hooks`

Hooks allow to run different instructions during certain boot stages. Used for example to pull up an IO during boot.

### Commands

| Command             | Signature                                                                                                                | Supported platforms |
|:--------------------|--------------------------------------------------------------------------------------------------------------------------|---------------------|
| `hooks.add_handler` | `(bootstage: int, handler: [() -> void]) -> void`<br/>Register a new hook. Different `bootstage` values described below. | All                 |                                                                                            | esp32               |

### enum `hooks.BOOTSTAGE`

| Command             | Description                                                               | Supported platforms |
|:--------------------|---------------------------------------------------------------------------|---------------------|
| `hooks.ON_INIT`     | Called earliest during boot, after scripting VM init.                     | All                 |
| `hooks.POST_SYSTEM` | Called after all core logic has been initialized, before plugins startup. | All                 |
| `hooks.POST_PLUGIN` | Called after all plugins have been initialized                            | All                 |
| `hooks.AP_INIT`     | Called after AP network has been initialized.                             | esp32               |
| `hooks.WIFI_INIT`   | Called after WiFi has been initialized.                                   | esp32               |


### Example

!!! example "Sample hook that runs after boot"

    Define I2S configuration, output 256 x MCLK clock on GPIO0.
    ```python
    hooks.add_handler(hooks.ON_INIT, def ()
        print("On boot called!")
    end)
    ```

## `i2s`

Controls I2S bus. Mainly used for DAC support.

**Implemented by `I2SDriver.cpp`**

### Commands

| Command              | Signature                                                                                                                       | Supported platforms |
|:---------------------|---------------------------------------------------------------------------------------------------------------------------------|---------------------|
| `i2s.install`        | `(configuration: I2SConfig) -> void`<br/>Installs I2S driver. See below for description of I2SConfig structure.                 | esp32               |
| `i2s.delete`         | `() -> void`.<br/>Uninstalls the current I2S driver.                                                                            | esp32               |
| `i2s.set_expand`     | `(src: int, dst: int) -> void`.<br/>Enables expand from `src` bits to `dst` bits in driver write. Useful for 32bit DAC support. | esp32               |
| `i2s.disable_expand` | `() -> void`.<br/>Disables bits expand.                                                                                         | esp32               |

### Object `I2SConfig`

| Field             | Description                                                                                                                                                 |
|:------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `sample_rate`     | `int`<br/>Defines sample rate for the incoming data signal. _WARNING - Sample rate is only initial, will be further changed in case od dynamic sample rate_ |
| `bits_per_sample` | `int`<br/>Bits per sample for incoming data.                                                                                                                |
| `comm_format`     | one of `I2S_CHANNEL_FMT_RIGHT_LEFT`, `I2S_CHANNEL_FMT_ALL_RIGHT`, `I2S_CHANNEL_FMT_ALL_LEFT` or `I2S_CHANNEL_FMT_ONLY_RIGHT`                                |
| `channel_format`  | one of `I2S_COMM_FORMAT_I2S`, `I2S_COMM_FORMAT_MSB`, `I2S_COMM_FORMAT_PCM_SHORT` or `I2S_COMM_FORMAT_PCM_LONG`                                              |
| `mclk`            | `int` <br/>if defined and larger than 0, outputs given `mclk` on GPIO0.                                                                                     |

### Example

!!! example "Sample driver configuration"

    Define I2S configuration, output 256 x MCLK clock on GPIO0.
    ```python
    var config = I2SConfig()
    config.sample_rate = 44100
    config.bits_per_sample = 16
    config.comm_format = I2S_CHANNEL_FMT_RIGHT_LEFT
    config.channel_format = I2S_COMM_FORMAT_I2S
    config.mclk = 256 * 44100

    i2s.install(config)
    ```
## `i2c`
Controls I2C bus on supported platforms. Mainly used in different drivers.

**Implemented by `I2CDriver.cpp`**

### Commands

| Command           | Signature                                                                                                                                                                  | Supported platforms |
|:------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| `i2c.install`     | `(sda: int, scl: int) -> void`<br/>Installs I2C driver under given pins.                                                                                                   | esp32               |
| `i2c.detect`      | `(addr:int) -> bool`.<br/>Tries to detect device under given addr. Returns true if device found.                                                                           | esp32               |
| `i2c.read_bytes`  | `(addr:int, reg:int, size:int) -> int or nil`.<br/>Read a value of 1..4 bytes from address addr and register reg. Returns nil if no response.                              | esp32               |
| `i2c.write_bytes` | `(addr:int, reg:int, val:bytes) -> nil`<br/>Writes the val bytes sequence as bytes() to address addr register reg.                                                         | esp32               |

### Example

## `gpio`
Controls GPIO pins on supported platforms. Mainly used in different drivers.

**Implemented by `GPIODriver.cpp`**

### Commands

| Command              | Signature                                                                                                                                                                                                                                                                                                          | Supported platforms |
|:---------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| `gpio.digital_write` | `(gpio: int, state: int) -> void`<br/>Sets GPIO to LOW/HIGH. Needs physical pin number                                                                                                                                                                                                                             | esp32               |
| `gpio.digital_read`  | `(gpio: int) -> int`<br/>Returns digital state of given physical GPIO. Either 0 or 1.                                                                                                                                                                                                                              | esp32               |
| `gpio.pin_mode`      | `(gpio: int, mode: int) -> int`<br/>Changes the GPIO mode. Only use if if you know what you're doing, by default Euphonium handles GPIO mode itself. Mode can have the following values: gpio.INPUT, gpio.OUTPUT, gpio.PULLUP, gpio.INPUT_PULLUP, gpio.PULLDOWN, gpio.OPEN_DRAIN, gpio.OUTPUT_OPEN_DRAIN, gpio.DAC | esp32               |
| `gpio.adc_voltage`   | `(gpio: int) -> real`.<br/>Returns the voltage on a given pin in mV. **Only used with DAC pins.**                                                                                                                                                                                                                  | esp32               |

### Example

## `wifi`

Controls internal state of the platform's WiFi. Used internally by `wifi.be`.

**Implemented by `WiFiDriver.cpp`**

### Commands

| Command                | Signature                                                                                                                                         | Supported platforms |
|:-----------------------|---------------------------------------------------------------------------------------------------------------------------------------------------|---------------------|
| `wifi.init_stack`      | `() -> void`<br/>Initializes the WiFi stack                                                                                                       | esp32               |
| `wifi.connect`         | `(ssid: string, password: string, fromAP: bool) -> void`<br/>Attempts WiFi connection. `fromAP` should be set according to the current WiFi mode. | esp32               |
| `wifi.start_ap`        | `(ssid: string, password: string) -> void`<br/>Starts an access point with given credentials.                                                     | esp32               |
| `wifi.start_scan`      | `() -> boid`.<br/>Starts scanning of WiFi networks.                                                                                               | esp32               |

### Example