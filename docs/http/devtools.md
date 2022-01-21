# DevTools Plugin API
Extension of Euphonium REST API. This plugin provides an API that allows for modification of the internal scripts, a functionality required by th web-ide.

## Events

This plugin does not provide custom events.

---------------------------------------

## DevTools

### `GET` `/devtools/file`
Returns list of all files available in the scripts filesystem.

#### Parameters
This endpoint does not take any parameters.

#### Response
??? example "Returns list of files"

    Response type `application/json`
    ```json
    [
        "app.be",
        "init.be",
        "esp32/dac.be"
    ]
    ```

### `POST` `/devtools/file/:filePath`
Updates content of the file under `filePath`. In case of file not existing, it creates one.

#### Parameters

| Parameter    | type   | Description                      |
|:-------------|--------|----------------------------------|
| `filePath`   | path   | Path to the file, including name |
| `body`       | body   | Contents of the file             |

#### Response
??? example "Returns list of files"

    Response type `application/json`
    ```json
    [
        "app.be",
        "init.be",
        "esp32/dac.be"
    ]
    ```

### `GET` `/devtools/logs`
Returns newest logfile.

#### Parameters
This endpoint does not take any parameters.

#### Response
??? example "Returns logs"

    Response type `text/plain`. This response is not a json, its just a list of logs split by a newline.
    ```
    I [cspot] ZeroconfAuthenticator.cpp:26: Got request for info
    I [cspot] ZeroconfAuthenticator.cpp:26: Got request for info
    I [cspot] ZeroconfAuthenticator.cpp:26: Got request for info
    I [persistor] ConfigPersistor.cpp:71: Sending file: index.html
    I [persistor] ConfigPersistor.cpp:71: Sending file: assets/index.4aec51d4.css
    I [persistor] ConfigPersistor.cpp:71: Sending file: assets/index.a6e012ba.js
    I [persistor] ConfigPersistor.cpp:71: Sending file: assets/vendor.0e1590e8.js
    ```

