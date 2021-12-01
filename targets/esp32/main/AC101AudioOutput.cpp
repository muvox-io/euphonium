#include "AC101AudioOutput.h"

AC101AudioOutput::AC101AudioOutput()
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), // Only TX
        .sample_rate = 44100,
        .bits_per_sample = (i2s_bits_per_sample_t)16,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, //2-channels
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0, //Default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = true,
        .tx_desc_auto_clear = true //Auto clear tx descriptor on underflow
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = 27,
        .ws_io_num = 26,
        .data_out_num = 25,
        .data_in_num = -1 //Not used
    };
    audioBuffer = (uint8_t*) heap_caps_malloc(4096, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);

    dac = &dac_a1s;

    dac->init(0, 0, &i2s_config);
    dac->speaker(false);
    dac->power(ADAC_ON);

    dac->volume(200, 200);
}

AC101AudioOutput::~AC101AudioOutput()
{
}

void AC101AudioOutput::feedPCMFrames(uint8_t* data, size_t nBytes) {
    memcpy(audioBuffer, data, nBytes);
    size_t written = 0;
    while (written < nBytes)
    {
        i2s_write((i2s_port_t)0, audioBuffer+written, nBytes - written, &written, portMAX_DELAY);
    }
}