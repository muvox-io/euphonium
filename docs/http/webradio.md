# WebRadio Plugin API
Extension of Euphonium REST API. This plugin provides playback of different HTTP audio streams.

## Events

This plugin does not provide custom events.

---------------------------------------

## WebRadio


### `POST` `/webradio`
In case of another audio source playing, this triggers the web radio source and plays radio with given parameters.

#### Parameters

| Parameter    | type | Description                             |
|:-------------|------|-----------------------------------------|
| `body`       | body | `WebRadio`<br/>Content described below. |

#### Requests

??? example "Set radio url to play"

    Request type `application/json`.
    ```json
    {
        "coverImage": "http://example.com/radio.jpeg",
        "url": "http://example.com/radio.mp3",
        "codec": "MP3",
        "name": "Example Radio",
    }
    ```

#### Responses

??? example "Returns set radio settings"

    Response type `application/json`.
    ```json
    {
        "coverImage": "http://example.com/radio.jpeg",
        "url": "http://example.com/radio.mp3",
        "codec": "MP3",
        "name": "Example Radio",
    }
    ```
