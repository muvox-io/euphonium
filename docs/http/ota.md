# OTA Plugin API
Extension of Euphonium REST API. This plugin provides an API for OTA updates on the `esp32` platform.

## Events

This plugin does not provide custom events.

---------------------------------------

## OTA

### `POST` `/ota`
This endpoint sets the URL and `sha256` of the firmware image to be downloaded, and reboots the device into recovery.

#### Parameters

| Parameter    | type | Description                        |
|:-------------|------|------------------------------------|
| `body`       | body | `OTA`<br/>Content described below. |

#### Requests

??? example "Set OTA url and it's sha256"

    Request type `application/json`.
    ```json
    {
        "url": "https://github.com/feelfreelinux/euphonium/releases/download/v0.0.14/ota.bin",
        "sha256": "e25ec6490d7cf480a26b5f18d7a50b8a7db247b18da749f3d938e8581bf92e22"
    }
    ```

#### Responses

??? example "Returns update OTA manifest"

    Response type `application/json`
    ```json
    {
        "url": "https://github.com/feelfreelinux/euphonium/releases/download/v0.0.14/ota.bin",
        "sha256": "e25ec6490d7cf480a26b5f18d7a50b8a7db247b18da749f3d938e8581bf92e22"
    }
    ```
