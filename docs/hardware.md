# Hardware
Euphonium has been confirmed to work fine with a bunch of hardware.

!!! Warning "ESP32-Specific"
    Please note that Euphonium officially works on any Linux / MacOS machine. This list is only specific to the ESP32 platform.

## Officially supported boards
All of following boards have a automatic configuration support in he web interface.

### Ai-Thinker ESP32 Audio-Kit

These boards are widely available on different stores for around 10 EUR. 

!!! Warning "Codec differences"
    Older versions of this board come with an `AC101` audio codec, while newer ones come with `ES8311`. Both versions are supported by Euphonium.

<img src="docs/images/a1s-kit.webp alt="a1s audio kit" width="200"/>

### Espressif Lyra-T v4.3

Official Espressif's devboard. Powered by `ES8311` codec.

<img src="docs/images/a1s-kit.webp alt="a1s audio kit" width="200"/>


## Officially supported DACs
These dacs are supported by euphonium, but require manual pin configuration.

### ES8311

### AC101

### TAS5711

### Internal DAC of ESP32

### MA12070p and MA12040p