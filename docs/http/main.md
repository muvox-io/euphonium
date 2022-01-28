# Euphonium REST API
Every instance of the player exposes a HTTP API, internally used by the web-ui and the web-ide. This API is accessible under the default port.

!!! warning "Warning"
    This API is not stable yet, and may change in the future. In particular, authentication is not implemented at this moment.

## Events

Euphonium's HTTP server exposes a endpoint for receiving live events. The events are sent as JSON objects. This is implemented as a long-lived connection to the `/events` endpoint. This is used for real time updates in the web ide.

---------------------------------------

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

## Playback

Manages the playback state of the system.

---------------------------------------

### `GET` `/playback`
Returns complete current state of playback, containing current track, current position, current volume and current eq settings.

#### Parameters
This endpoint does not have parameters

#### Responses

??? example "Returns current playback state"

    Response type `application/json`. Example of spotify.
    ```json
    {
        "song": {
            "icon": "https://i.scdn.co/image/kocz.jpg",
            "songName": "Never Gonna Give You Up",
            "albumName": "Never Gonna Give You Up",
            "sourceName": "cspot",
            "artistName": "Rick Astley",
        },
        "status": "paused",
        "volume": 50,
        "eq": {
            "low": 0,
            "mid": -0.5,
            "high": 0
        }
    }
    ```

### `POST` `/playback/volume`
Updates playback current volume.

#### Parameters

| Parameter    | type | Description                           |
|:-------------|------|---------------------------------------|
| `body`       | body | `Volume`<br/>Content described below. |

#### Requests

??? example "Change volume"

    Request type `application/json`. Volume is an integer betwen 0 and 100.
    ```json
    {
        "volume": 100,
    }
    ```

#### Responses

??? example "Returns current playback state"

    Response type `application/json`. Example of spotify.
    ```json
    {
        "song": {
            "icon": "https://i.scdn.co/image/kocz.jpg",
            "songName": "Never Gonna Give You Up",
            "albumName": "Never Gonna Give You Up",
            "sourceName": "cspot",
            "artistName": "Rick Astley",
        },
        "status": "paused",
        "volume": 100,
        "eq": {
            "low": 0,
            "mid": -0.5,
            "high": 0
        }
    }
    ```

### `POST` `/playback/eq`
Updates playback current equalizer settings.

#### Parameters

| Parameter    | type | Description                       |
|:-------------|------|-----------------------------------|
| `body`       | body | `Eq`<br/>Content described below. |

#### Requests

??? example "Change equalizer settings"

    Request type `application/json`. Equalizer settings are an object with keys `low`, `mid` and `high`. One value on the scale corresponds to 3 dB.
    ```json
    {
        "low": 1.25,
        "mid": 0,
        "high": 0
    }
    ```

#### Responses

??? example "Returns current playback state"

    Response type `application/json`. Example of spotify.
    ```json
    {
        "song": {
            "icon": "https://i.scdn.co/image/kocz.jpg",
            "songName": "Never Gonna Give You Up",
            "albumName": "Never Gonna Give You Up",
            "sourceName": "cspot",
            "artistName": "Rick Astley",
        },
        "status": "paused",
        "volume": 100,
        "eq": {
            "low": 1.25,
            "mid": 0,
            "high": 0
        }
    }
    ```

### `POST` `/playback/status`
Updates playback current playback status.

#### Parameters

| Parameter    | type | Description                             |
|:-------------|------|-----------------------------------------|
| `body`       | body | `Playback`<br/>Content described below. |

#### Requests

??? example "Change playback settings"

    Request type `application/json`. Status is either 'playing' or 'paused'
    ```json
    {
        "status": "playing"
    }
    ```

#### Responses

??? example "Returns current playback state"

    Response type `application/json`. Example of spotify.
    ```json
    {
        "song": {
            "icon": "https://i.scdn.co/image/kocz.jpg",
            "songName": "Never Gonna Give You Up",
            "albumName": "Never Gonna Give You Up",
            "sourceName": "cspot",
            "artistName": "Rick Astley",
        },
        "status": "playing",
        "volume": 100,
        "eq": {
            "low": 1.25,
            "mid": 0,
            "high": 0
        }
    }
    ```

## System

Manages the state of the system.

---------------------------------------

### `GET` `/system`
Returns information about current system state.

#### Parameters
This endpoint does not have parameters

#### Responses

??? example "Returns current system state"

    Response type `application/json`.
    ```json
    {
        "networkState": "online",
        "version": "0.0.14"
    }
    ```

### `POST` `/system/reboot`
Returns information about current system state.

#### Parameters
Calling this endpoint will reboot the system.

#### Responses

??? example "Returns current system state"

    Response type `application/json`.
    ```json
    {
        "version": "0.0.14",
        "networkState": "rebooting",
    }
    ```

## WiFi

Manages state of the WiFi connection

---------------------------------------

### `GET` `/wifi`
Returns current WiFi state.

#### Parameters
This endpoint does not have parameters

#### Responses

??? example "Returns current WiFi state"

    Response type `application/json`. Example of connected to AP.
    ```json
    {
        "state": "connected",
        "ipAddress": "192.168.254.103"
    }
    ```

### `POST` `/wifi/connect`
Connects to WiFi network.

#### Parameters

| Parameter    | type | Description                                 |
|:-------------|------|---------------------------------------------|
| `body`       | body | `WiFiSettings`<br/>Content described below. |

#### Requests

??? example "WiFi network parameters"

    Request type `application/json`.
    ```json
    {
        "ssid": "RandomNetwork",
        "password": "Test123"
    }
    ```


#### Responses

??? example "Returns current WiFi state"

    Response type `application/json`. Example of connected to AP.
    ```json
    {
        "state": "connecting",
    }
    ```


### `POST` `/wifi/scan_start`
Starts WiFi network scan.

#### Parameters

This endpoint does not have parameters

#### Responses

??? example "Returns current WiFi state"

    Response type `application/json`. Example of connected to AP.
    ```json
    {
        "state": "scanning",
    }
    ```