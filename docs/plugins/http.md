# Euphonium REST API
Every instance of the player exposes a HTTP API, internally used by the web-ui and the web-ide. This API is accessible under the default port.

!!! warning "Warning"
    This API is not stable yet, and may change in the future. In particular, authentication is not implemented at this moment.

## Plugins

Manages internal plugins.

---------------------------------------

### `GET` `/plugins`
Returns list of currently active plugins.

#### Parameters
This endpoint does not have parameters

#### Responses

??? example "Returns list of plugins"

    Response type `application/json`
    ```json
    [
        {
            "type": "plugin",
            "name": "cspot",
            "displayName": "Spotify (cspot)"
        },
        {
            "type": "plugin",
            "name": "webradio",
            "displayName": "WebRadio"
        },
        {
            "type": "app",
            "name": "webradio",
            "displayName": "WebRadio"
        },
    ]
    ```

---------------------------------------

### `GET` `/plugins/:pluginName`
Returns configuration schema with current values for given plugin.

#### Parameters

| Parameter    | type | Description                                                   |
|:-------------|------|---------------------------------------------------------------|
| `pluginName` | path | `string`<br/>Name of the plugin to retrieve configuration of. |

#### Responses

??? example "Returns plugin configuration"

    Response type `application/json`. Example of spotify.
    ```json
    {
        "configSchema": {
            "audioBitrate": {
                "value": "160",
                "tooltip": "Audio bitrate",
                "type": "stringList",
                "defaultValue": "160",
                "listValues": [ "96", "160", "320" ]
            },
            "receiverName": {
                "tooltip": "Speaker's name",
                "type": "string",
                "value": "Euphonium (cspot)",
                "defaultValue": "Euphonium (cspot)"
            }
        },
        "themeColor": "#1DB954",
        "displayName": "Spotify (cspot)"
    }
    ```

### `POST` `/plugins/:pluginName`
Updates configuration of given plugin schema. This configuration is persisted between reboots.

#### Parameters

| Parameter    | type | Description                                                   |
|:-------------|------|---------------------------------------------------------------|
| `pluginName` | path | `string`<br/>Name of the plugin to retrieve configuration of. |
| `body`       | body | `PluginConfig`<br/>Content described below.                   |

#### Requests

??? example "Change spotify configuration"

    Response type `application/json`.
    ```json
    {
        "audioBitrate": "320",
        "receiverName": "Living room\'s speaker"
    }
    ```

#### Responses

??? example "Returns plugin configuration"

    Request type `application/json`. Example of spotify.
    ```json
    {
        "configSchema": {
            "audioBitrate": {
                "value": "320",
                "tooltip": "Audio bitrate",
                "type": "stringList",
                "defaultValue": "160",
                "listValues": [ "96", "160", "320" ]
            },
            "receiverName": {
                "tooltip": "Speaker's name",
                "type": "string",
                "value": "Living room\'s speaker",
                "defaultValue": "Euphonium (cspot)"
            }
        },
        "themeColor": "#1DB954",
        "displayName": "Spotify (cspot)"
    }
    ```