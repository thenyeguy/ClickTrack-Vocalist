#include <iostream>
#include "portaudio_wrapper.h"

using namespace ClickTrack;


int pa_error_check(std::string location, int err)
{
    if(err == paNoError) return err;

    std::cout << "An error occured during " << location << ": " << 
        Pa_GetErrorText(err) << "\n";
    exit(err);
}


InputStream::InputStream(unsigned in_channels)
    : channels(in_channels)
{
    // Initialize portaudio
    pa_error_check("PaInitialize", Pa_Initialize());

    //Use default input and output devices on the computer
    PaStreamParameters inputParams;

    inputParams.device = Pa_GetDefaultInputDevice();
    inputParams.channelCount = channels;
    inputParams.sampleFormat = PA_SAMPLE_TYPE;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    //Open the stream!
    pa_error_check("Pa_OpenStream",
        Pa_OpenStream(&stream, &inputParams, NULL,
            SAMPLE_RATE, PORTAUDIO_BUFFER_SIZE, paNoFlag,
            NULL, NULL));
    pa_error_check("Pa_StartStream", Pa_StartStream(stream));
}


InputStream::~InputStream()
{
    pa_error_check("Pa_StopStream", Pa_StopStream(stream));
    pa_error_check("Pa_CloseStream", Pa_CloseStream(stream));
    pa_error_check("Pa_Terminate", Pa_Terminate());
}


OutputStream::OutputStream(unsigned in_channels)
    : channels(in_channels)
{
    // Initialize portaudio
    pa_error_check("PaInitialize", Pa_Initialize());

    //Use default input and output devices on the computer
    PaStreamParameters outputParams;

    outputParams.device = Pa_GetDefaultOutputDevice();
    outputParams.channelCount = channels;
    outputParams.sampleFormat = PA_SAMPLE_TYPE;
    outputParams.suggestedLatency =
        Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

    //Open the stream!
    pa_error_check("Pa_OpenStream",
        Pa_OpenStream(&stream, NULL, &outputParams,
            SAMPLE_RATE, PORTAUDIO_BUFFER_SIZE, paNoFlag,
            NULL, NULL));
    pa_error_check("Pa_StartStream", Pa_StartStream(stream));
}


OutputStream::~OutputStream()
{
    pa_error_check("Pa_StopStream", Pa_StopStream(stream));
    pa_error_check("Pa_CloseStream", Pa_CloseStream(stream));
    pa_error_check("Pa_Terminate", Pa_Terminate());
}


void InputStream::readFromStream(SAMPLE* buffer, int num_samples)
{
    Pa_ReadStream(stream, buffer, num_samples);    
}


void OutputStream::writeToStream(SAMPLE* buffer, int num_samples)
{
    Pa_WriteStream(stream, buffer, num_samples);
}
