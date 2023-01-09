#include "I2SAudioOutput.h"

using namespace euph;

I2SAudioOutput::I2SAudioOutput(){};

void I2SAudioOutput::setupBindings(std::shared_ptr<euph::Context> ctx) {
  ctx->vm->export_this("set_pins", this, &I2SAudioOutput::_setPins, "i2s");
  ctx->vm->export_this("uninstall", this, &I2SAudioOutput::_uninstall, "i2s");
  ctx->vm->export_this("install", this, &I2SAudioOutput::_install, "i2s");
}

bool I2SAudioOutput::supportsHardwareVolume() {
  return true;
}

void I2SAudioOutput::configure(uint32_t sampleRate, uint8_t channels,
                               uint8_t bitwidth) {}

void I2SAudioOutput::setVolume(uint8_t volume) {}

void I2SAudioOutput::feedPCM(uint8_t* pcm, size_t size) {}

void I2SAudioOutput::_install(int sampleRate, int bitsPerSample,
                              int channelFormatInt, int commFormat, int mclk) {
  i2s_channel_fmt_t channelFormat;
  switch (channelFormatInt) {
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
      .sample_rate = (uint32_t)sampleRate,
      .bits_per_sample = (i2s_bits_per_sample_t)bitsPerSample,
      .channel_format = channelFormat,
      .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0,  // Default interrupt priority
      .dma_buf_count = 8,
      .dma_buf_len = 512,
      .use_apll = true,
      .tx_desc_auto_clear = true,  // Auto clear tx descriptor on underflow
  };

  if (mclk > 0) {
    i2s_config.fixed_mclk = mclk * sampleRate;
  }

  i2s_driver_install((i2s_port_t)0, &i2s_config, 0, NULL);
}
void I2SAudioOutput::_setPins(int mck, int bck, int ws, int dataOut) {
  EUPH_LOG(info, "i2s", "Setting pins %d %d %d %d", mck, bck, ws, dataOut);
  i2s_pin_config_t pin_config = {
      .mck_io_num = mck,
      .bck_io_num = bck,
      .ws_io_num = ws,
      .data_out_num = dataOut,
      .data_in_num = -1  // Not used
  };

  i2s_set_pin((i2s_port_t)0, &pin_config);
}

void I2SAudioOutput::_uninstall() {
  i2s_driver_uninstall((i2s_port_t)0);
}
