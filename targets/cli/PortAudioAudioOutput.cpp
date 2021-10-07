#include "PortAudioAudioOutput.h"

PortAudioAudioOutput::PortAudioAudioOutput() {
    Pa_Initialize();
    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice) {
        printf("PortAudio: Default audio device not found!\n");
        // exit(0);
    }
        printf("PortAudio: Default audio device not found!\n");

    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paInt16; /* 32 bit floating point output */
    outputParameters.suggestedLatency = 0.050;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(
        &stream,
        NULL,
        &outputParameters,
        44100,
        4096 / 4,
        paClipOff,
        NULL, // blocking api
        NULL
    );
    Pa_StartStream(stream);
}

PortAudioAudioOutput::~PortAudioAudioOutput()
{
    Pa_StopStream(stream);
    Pa_Terminate();
}

void PortAudioAudioOutput::update(std::shared_ptr<CircularBuffer> audioBuffer) {
    auto buffer = std::vector<uint8_t>(4096);
    auto readNumber = audioBuffer->read(buffer.data(), 4096);
    Pa_WriteStream(stream, &buffer[0], buffer.size() / 4);
}