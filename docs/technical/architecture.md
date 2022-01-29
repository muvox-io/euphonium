# Architecture

## Plugins

## Scripting language structure

The following table describes folder structure of the internal scripting file system.

### Folder `internal`

**Rewritten by OTA**

Contains the core of the application. This handles the central classes, main http endpoints, and is the main entrypoint.


| File                    | Description                                                                                            |
|:------------------------|--------------------------------------------------------------------------------------------------------|
| `internal/init.be`      | Main entrypoint of the script loader. **Do not add your extensions here, please see `extensions.be`**. |
| `internal/api.be`       | Implementation of the main Euphonium REST API.                                                         |
| `internal/bindings.be`  | Shallow binding classes that allow for communication between Berry and C++.                            |
| `internal/euphonium.be` | Main Euphonium class which handles system initialization, event handling and plugin registration.      |
| `internal/http.be`      | Sugar syntax wrapper over the native HTTP server.                                                      |
| `internal/app-version`  | Text file containing current version, needed for OTA.                                                  |

### Folder `vendor`

**Rewritten by manufacturer OTA**

Contains vendor-specific code. This allows for customization of euphonium for different manufacturers. Code here is rewritten with every manufacturer OTA.

- `vendor/init.be` - Entry point for all vendor-specific code.

Rest of the code here is product-specific.


### Folder `configuration`

Contains saved configuration of the app in form of `*.config.json` files.

### Folder `esp32`

Contains the ESP32 platform support. **TODO: Describe structure**.

### Folder `extensions`

Contains all user scripts. These are loaded last, after all other scripts. You can register your custom driver by appending to `extensions.be` file.