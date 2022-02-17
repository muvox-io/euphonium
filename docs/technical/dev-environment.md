# Setting up development environment
The best way to try euphonium is to flash a prebuilt version. However, if you want to build it yourself, or help out with development, you will need a few dependencies to get this project to build.

## Setup

### Setting up web UI bundler

First, a required step is to setup all of the dependencies required to build the web UI.

#### Dependencies
- `nodejs` in version of at least 14
- `yarn`

Installation of both is platform specific, but mostly just comes down to installing them through a package manager.

### Setting up dependencies

Depending on the target platform, esp32 or cli targets differ in required dependencies.

#### Both platforms
- `python` with `pip`
- `pip3 install grpcio-tools protobuf`

#### Desktop specific dependencies
- PortAudio library
- OpenSSL library

both can be installed with a package manager.

#### ESP32-Specific dependencies
- `esp-idf` in version `4.4-rc1`. Please follow [Espressif's guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#get-started-get-prerequisites).


### Building and installing the project - Desktop

For a desktop run, please run the following commands

!!! note "Setup"
    ```
    cd targets/cli
    mkdir build && cd build
    cmake ..
    ```

!!! note "Build"
    ```
    make
    ```


This will output a binary `euphoniumcli` which can be later executed to run the platform. The web-ui will be available on port `80`.

### Building and installing the project - ESP32

For ESP32 target, please run following commands (with esp-idf in the PATH)

!!! note "Setup"
    ```
    cd targets/esp32
    sh build_recovery.sh 
    sh build_app.sh
    ```

!!! note "Build"
    For full flash:
    ```
    sh flash_all.sh {serialport}
    ```
    For app-only flash:
    ```
    sh reflash_app.sh {serialport}
    ```
    For spiffs only flash (berry scripts, webui, configuration):
    ```
    sh reflash_spiffs.sh {serialport}
    ```

## Development improvements

There are a few tricks to make the development faster

### Preconfiguration of WIFI and DAC

Every flash of the spiffs partition will reset the internal configuration. It's a bit of a pain when dealing with esp32, as you have to deal with reconfiguration of the system through the AP network with every flash. To avoid this, you can preconfigure the WiFi settings (any any other setting!) to be included in the flashed partition.

To configure WiFi, create a file inside of `euphonium/scripts` folder, named `wifi.config.json`, containing following data

```
{
    "ssid": "YOUR NETWORK SSID",
    "password": " YOUR NETWORK PASSWORD"
}
```

After rerunning the `reflash_spiffs` script WiFi is going to be preconfigured :)
