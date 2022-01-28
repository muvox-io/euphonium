#include "I2SDriver.h"

void i2sInstall(int sampleRate, int bitsPerSample, int channelFormatInt, int commFormat, int mclk)
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
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = (uint32_t) sampleRate,
        .bits_per_sample = (i2s_bits_per_sample_t)bitsPerSample,
        .channel_format = channelFormat,
        .communication_format = (i2s_comm_format_t)commFormat,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = true,
        .tx_desc_auto_clear = true, // Auto clear tx descriptor on underflow
    };

    if (mclk > 0) {
        i2s_config.fixed_mclk = mclk * sampleRate;
    }

    i2s_driver_install((i2s_port_t)0, &i2s_config, 0, NULL);
}

// Set Pin configuration
void i2sSetPins(int mck, int bck, int ws, int dataOut) {
    BELL_LOG(info, "i2s", "Setting pins %d %d %d %d", mck, bck, ws, dataOut);
    i2s_pin_config_t pin_config = {
        .mck_io_num = mck,
        .bck_io_num = bck,
        .ws_io_num = ws,
        .data_out_num = dataOut,
        .data_in_num = -1 // Not used
    };

    i2s_set_pin((i2s_port_t)0, &pin_config);
}

void InternalDACInstall(int channelFormatInt, int sampleRate)
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
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN), // Only TX, Internal DAC
        .sample_rate = (uint32_t) sampleRate,
        .bits_per_sample = (i2s_bits_per_sample_t)16,
        .channel_format = channelFormat, // 2-channels
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = true,
        .tx_desc_auto_clear = true, // Auto clear tx descriptor on underflow
        .fixed_mclk=-1
    };

    i2s_driver_install((i2s_port_t)0, &i2s_config, 0, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
    i2s_set_pin((i2s_port_t)0, NULL);
}

void i2sEnableMCLK() {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    WRITE_PERI_REG(PIN_CTRL, 0xFFF0);
}

void i2sUninstall() {
    i2s_driver_uninstall((i2s_port_t) 0);
}

void exportI2SDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("install", &i2sInstall, "i2s");
    berry->export_function("internalDAC_install", &InternalDACInstall);
    berry->export_function("uninstall", &i2sUninstall, "i2s");
    berry->export_function("set_pins", &i2sSetPins, "i2s");
}
