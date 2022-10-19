#include "PortAudioAudioOutput.h"

PortAudioAudioOutput::PortAudioAudioOutput() {
    Pa_Initialize();
    configureOutput(AudioOutput::SampleFormat::INT16, 44100);
}

PortAudioAudioOutput::~PortAudioAudioOutput()
{
    Pa_StopStream(stream);
    Pa_Terminate();
}

void PortAudioAudioOutput::configureOutput(SampleFormat format, int sampleRate) {
    if (stream != NULL) Pa_StopStream(stream);

    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice) {
        printf("PortAudio: Default audio device not found!\n");
        // exit(0);
    }
    printf("PortAudio: Default audio device not found!\n");

    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.suggestedLatency = 0.050;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    if (format == AudioOutput::SampleFormat::INT16) {
        outputParameters.sampleFormat = paInt16;
    }

    if (format == AudioOutput::SampleFormat::INT32) {
        outputParameters.sampleFormat = paInt32;
    }

    PaError err = Pa_OpenStream(
        &stream,
        NULL,
        &outputParameters,
        sampleRate,
        4096 / 4,
        paClipOff,
        NULL, // blocking api
        NULL
    );
    Pa_StartStream(stream);
}

void PortAudioAudioOutput::feedPCMFrames(uint8_t* data, size_t nBytes) {
    Pa_WriteStream(stream, data, nBytes / 4);
}