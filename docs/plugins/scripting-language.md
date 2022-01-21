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
| `http.write_text` | `(body: string, fd: int, status: int) -> void`.<br/>Writes given response with type `text/plain`. `fd` is connection fd taken from the `HTTPRequest` object.                                                                             | All                 |
| `http.write_json` | `(body: map, fd: int, status: int) -> void`.<br/>As above, but serializes the `body` into a json string. Content-type is `application/json`.                                                                                             | All                 |
| `http.emit_event` | `(type: string, body: map) -> void`.<br/>Broadcasts a server-side event to all connected devices. `body` will be serialized into json string.                                                                                            | All                 |

### Object `HTTPRequest`

### Object `HTTPResponse`

### Example

!!! example "HTTP server usage"

    Handle simple GET and return "Hello, world!"

    ```python
    http.handle('GET', '/hello_world', def (request)
        http.write_text("Hello world!", request['connection'], 200)
    end)
    ```
    Handle POST with json body, return a json response

    ```python
    http.handle('POST', '/create_cat', def (request)
        if request['body'] == nil
            http.write_text("No body", request['connection'], 400)
        else
           # Parse json body
           var parsedBody = json.parse(request['body'])

           # Create response
           var response = { 'name': parsedBody['name'], 'age': 3 }
           http.write_json(response, request['connection'], 200)
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
| `i2c.read`        | `(addr:int, reg:int, size:int) -> int or nil`.<br/>Read a value of 1..4 bytes from address addr and register reg. Returns nil if no response.                              | esp32               |
| `i2c.write`       | `(addr:int, reg:int, val:int, size:int) -> bool`.<br/>Writes a value of 1..4 bytes to address addr, register reg with value val. Returns true if successful, false if not. | esp32               |
| `i2c.read_bytes`  | `(addr:int, reg:int, size:int) -> int or nil`.<br/>Read a value of 1..4 bytes from address addr and register reg. Returns nil if no response.                              | esp32               |
| `i2c.write_bytes` | `(addr:int, reg:int, val:bytes) -> nil`<br/>Writes the val bytes sequence as bytes() to address addr register reg.                                                         | esp32               |

### Example

## GPIO

## WiFi