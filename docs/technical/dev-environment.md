

# Setup
The best way to try euphonium is to flash a prebuilt version. However, if you want to build it yourself, or help out with development, you will need a few dependencies to get this project to build.

## Checkout code

When checking out the repository, use `--recursive` to retrieve all submodules. Alternately, after checkout use `git submodule update --init --recursive` to perform the same task.

```
git clone --recursive https://github.com/feelfreelinux/euphonium
```

## Setting up web UI bundler

First, a required step is to setup all of the dependencies required to build the web UI.

### python
Python needs to be installed on your system.
Ensure that you have `pip` installed. This can be done by

```
python -m ensurepip --upgrade
```

Then we can install the needed dependencies by using the `requirements.txt` file. Execute this command to install the needed dependencies (from the root of the project):

```
pip install -r requirements.txt
```


### nodejs
you need at least version 14. Precompiled distrobution packages with installation instructions can be found here: https://github.com/nodesource/distributions

### npm
Npm should be included in the nodesource package. Otherwise the installation is platform specific, but mostly just comes down to installing it through a package manager.

### yarn
be careful: don't install yarn from default package repository on Debian or Ubuntu. It would install the package `cmdtest` for you. To be sure you may run `sudo apt remove cmdtest` first.

The right package can be installed with:
```
corepack enable
yarn set version stablerm -rf bu        
```

## Setting up dependencies for target platform

Depending on the target platform, (esp32 or cli) the dependencies are differnt.


### Desktop specific dependencies

- PortAudio library
- OpenSSL library

both can be installed with a package manager.

If you are on macOS then we can do this with `brew`:
```
brew install OpenSSL PortAudio
```

#### Needed libs on Linux
If you are on Ubuntu/Debian you can install this with:
```
sudo apt install libmbedtls-dev protobuf-compiler openssl libasound-dev portaudio19-dev libportaudio2 libportaudiocpp0
```

When building on linux you will likely also need the follwoing dependency: `libavahi-compat-libdnssd-dev`.
This can be installed by
```
sudo apt-get install libavahi-compat-libdnssd-dev
```

both can be installed with a package manager.

#### ESP32-Specific dependencies

- `esp-idf` in version `4.4.1`. Please follow [Espressif's guide](https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/get-started/index.html#get-started-get-prerequisites).

As an alternative you can install VSCode (https://code.visualstudio.com/) and open the euphonium folder.
VSCode should recommend the following extensions to you:
- "espressif.esp-idf-extension"
- "platformio.platformio-ide" (optional)
- "berry.berry" (optional)

In this case the installation of ESP-IDF is managed by vscode.  
If the extensions are installed you can open the ESP-IDF terminal (`STRG + E` followed by `T`) and continue building as described below.

#### Hint for Linux Mint
In order for the CLI build to work on Linux Mint I had to add a little extra CLI arg to the `cmake` command, as it doesn't know how to find the correct library it would seem.
Maybe this could be added directly into the `cmake` setup?

To fully compile the `cli` on Linux Mint execute the following
```
cmake -DCMAKE_CXX_STANDARD_LIBRARIES="-ldl" ..
make
```

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


This will output a binary `euphoniumcli` which can be later executed to run the platform. The web-ui will be available on port `80` by default.
If you have trouble using port 80 (on Linux for example), use `cmake .. -D HTTP_SERVER_PORT=8080` to change the web-ui port.

#### Hint for Linux Mint
In order for the CLI build to work on Linux Mint I had to add a little extra CLI arg to the `cmake` command, as it doesn't know how to find the correct library it would seem.
Maybe this could be added directly into the `cmake` setup?

To fully compile the `cli` on Linux Mint execute the following
```
cmake -DCMAKE_CXX_STANDARD_LIBRARIES="-lX11 -ldl" ..
make
```

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

To configure WiFi, create a file inside of `euphonium/scripts/configuration` folder, named `wifi.config.json`, containing following data

```
{
    "ssid": "YOUR NETWORK SSID",
    "password": " YOUR NETWORK PASSWORD"
}
```

After rerunning the `reflash_spiffs` script WiFi is going to be preconfigured :)
