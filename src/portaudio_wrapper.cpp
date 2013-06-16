#include <iostream>
#include "portaudio_wrapper.h"

using namespace std;
using namespace Portaudio;


int error_check(string location, int err)
{
    if(err == paNoError) return err;

    cout << "An error occured during " << location << ": " << 
        Pa_GetErrorText(err) << "\n";
    exit(err);
}


InputStream::InputStream()
{
    // Initialize portaudio
    error_check("PaInitialize", Pa_Initialize());

    //Use default input and output devices on the computer
    PaStreamParameters inputParams;

    inputParams.device = Pa_GetDefaultInputDevice();
    inputParams.channelCount = 1;
    inputParams.sampleFormat = PA_SAMPLE_TYPE;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    //Open the stream!
    error_check("Pa_OpenStream",
        Pa_OpenStream(&stream, &inputParams, NULL,
            DEFAULT_SAMPLE_RATE, DEFAULT_BUFFER_SIZE, paNoFlag,
            NULL, NULL));
    error_check("StartStream", Pa_StartStream(stream));
}


InputStream::~InputStream()
{
    error_check("StopStream", Pa_StartStream(stream));
    error_check("Pa_CloseStream", Pa_CloseStream(&stream));
    error_check("Pa_Terminate", Pa_Terminate());
}


OutputStream::OutputStream()
{
    // Initialize portaudio
    error_check("PaInitialize", Pa_Initialize());

    //Use default input and output devices on the computer
    PaStreamParameters outputParams;

    outputParams.device = Pa_GetDefaultOutputDevice();
    outputParams.channelCount = 1;
    outputParams.sampleFormat = PA_SAMPLE_TYPE;
    outputParams.suggestedLatency =
        Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

    //Open the stream!
    error_check("Pa_OpenStream",
        Pa_OpenStream(&stream, NULL, &outputParams,
            DEFAULT_SAMPLE_RATE, DEFAULT_BUFFER_SIZE, paNoFlag,
            NULL, NULL));
    error_check("StartStream", Pa_StartStream(stream));
}


OutputStream::~OutputStream()
{
    error_check("StopStream", Pa_StartStream(stream));
    error_check("Pa_CloseStream", Pa_CloseStream(&stream));
    error_check("Pa_Terminate", Pa_Terminate());
}


void InputStream::readFromStream(SAMPLE* buffer, int num_samples)
{
    Pa_ReadStream(stream, buffer, num_samples);    
}


void OutputStream::writeToStream(SAMPLE* buffer, int num_samples)
{
    Pa_WriteStream(stream, buffer, num_samples);
}
