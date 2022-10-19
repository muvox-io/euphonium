#include "PortAudioSink.h"

PortAudioSink::PortAudioSink()
{
    Pa_Initialize();
	this->initialize(44100);
}

void PortAudioSink::initialize(uint16_t sampleRate) {
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
		sampleRate,
        4096 / 4,
        paClipOff,
        NULL, // blocking api
        NULL
    );
    Pa_StartStream(stream);
}

PortAudioSink::~PortAudioSink()
{
    Pa_StopStream(stream);
    Pa_Terminate();
}

bool PortAudioSink::setRate(uint16_t sampleRate) {
	if (Pa_GetStreamInfo(stream)->sampleRate != sampleRate) {
		Pa_StopStream(stream);
		this->initialize(sampleRate);
	}
	return true;
}

void PortAudioSink::feedPCMFrames(const uint8_t *buffer, size_t bytes)
{
    Pa_WriteStream(stream, buffer, bytes / 4);
}
