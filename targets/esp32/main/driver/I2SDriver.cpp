#include "I2SDriver.h"

void i2sInstall(int channelFormatInt, int commFormat, int sampleRate, bool autoClear, int bck, int ws, int dataOut)
{
    i2s_channel_fmt_t channelFormat;
    switch (channelFormatInt)
    {
    case 0:
        channelFormat = I2S_CHANNEL_FMT_RIGHT_LEFT;
        break;
    case 1:
        channelFormat = I2S_CHANNEL_FMT_ALL_RIGHT;
        break;
    case 2:
        channelFormat = I2S_CHANNEL_FMT_ALL_LEFT;
        break;
    case 3:
        channelFormat = I2S_CHANNEL_FMT_ONLY_RIGHT;
        break;
    default:
        channelFormat = I2S_CHANNEL_FMT_RIGHT_LEFT;
        break;
    }

    i2s_config_t i2s_config = {

        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), // Only TX
        .sample_rate = sampleRate,
        .bits_per_sample = (i2s_bits_per_sample_t)16,
        .channel_format = channelFormat, // 2-channels
        .communication_format = (i2s_comm_format_t)commFormat,
        .intr_alloc_flags = 0, // Default interrupt priority
        .dma_buf_count = 12,
        .dma_buf_len = 512,
        .use_apll = true,
        .tx_desc_auto_clear = autoClear, // Auto clear tx descriptor on underflow
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = bck,
        .ws_io_num = ws,
        .data_out_num = dataOut,
        .data_in_num = -1 // Not used
    };
    i2s_driver_install((i2s_port_t)0, &i2s_config, 0, NULL);
    i2s_set_pin((i2s_port_t)0, &pin_config);
}

void i2sDelete() {
    i2s_driver_uninstall((i2s_port_t) 0);
}

void exportI2SDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("i2s_install", &i2sInstall);
    berry->export_function("i2s_delete", &i2sDelete);
}
