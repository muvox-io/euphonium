# bell

Core audio utils library used in [cspot](https://github.com/feelfreelinux/cspot)
and [euphonium](https://github.com/feelfreelinux/euphonium) projects.

What's included:

- `HTTPServer`, `HTTPClient` - no need to explain
- `BufferedStream` - wrapper for `ByteStream` with a worker-thread ring buffer
- Crypto (mbedtls backed)
- Audio sinks interface:
    - for Unix/Cygwin: ALSA, PortAudio, named pipe outputs (see root CMakeLists for options)
    - for ESP32: DACs (`AC101`, `ES8388`, `ES9018`, `PCM5102`, `TAS5711`), S/PDIF over I2S, internal DAC output
    - → see `src/audio/sinks/esp` for pin configuration, etc.
- Abstraction layer over multiple audio codecs: (disable with `BELL_DISABLE_CODECS`)
    - AAC - using `libhelix-aac` - enabled with `BELL_CODEC_AAC`
    - MP3 - using `libhelix-mp3` - enabled with `BELL_CODEC_MP3`
    - Vorbis - using Xiph.org's `tremor` - enabled with `BELL_CODEC_VORBIS`
    - Opus - using Xiph.org's reference Opus impl - enabled with `BELL_CODEC_OPUS`
- Audio container reading classes, with random access support:
    - `Mpeg4Container` - supports MPEG-4 and QuickTime files, either with or without video, fragmented or not
- cJSON library with JSONObject wrapper (disable with `BELL_DISABLE_CJSON`)
- Logging utilities
- Nanopb protobuf library, for use in CMake
- Semaphore implementations for multiple platforms
