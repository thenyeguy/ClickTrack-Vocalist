#include <iostream>
#include "portaudio_wrapper.h"

using namespace std;


int error_check(string location, int err)
{
    if(err == paNoError) return err;

    cout << "An error occured during " << location << ": " << 
        Pa_GetErrorText(err) << "\n";
    exit(err);
}


PortaudioStream::PortaudioStream()
{
    // Initialize portaudio
    error_check("PaInitialize", Pa_Initialize());

    //Use default input and output devices on the computer
    PaStreamParameters inputParams, outputParams;

    inputParams.device = Pa_GetDefaultInputDevice();
    inputParams.channelCount = 1;
    inputParams.sampleFormat = PA_SAMPLE_TYPE;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = 0;

    outputParams.device = Pa_GetDefaultOutputDevice();
    outputParams.channelCount = 1;
    outputParams.sampleFormat = PA_SAMPLE_TYPE;
    outputParams.suggestedLatency =
        Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = 0;

    //Open the stream!
    error_check("Pa_OpenStream",
        Pa_OpenStream(&stream, &inputParams, &outputParams,
            PA_DEFAULT_SAMPLE_RATE, PA_DEFAULT_BUFFER_SIZE, paNoFlag,
            0, 0));
    error_check("StartStream", Pa_StartStream(stream));
    
    cout << "Opened portaudio stream.\n";
}


PortaudioStream::~PortaudioStream()
{
    error_check("StopStream", Pa_StartStream(stream));
    error_check("Pa_CloseStream", Pa_CloseStream(&stream));
    error_check("Pa_Terminate", Pa_Terminate());
    cout << "You are done!\n";
}


void PortaudioStream::readFromStream(float* buffer, int num_samples)
{
    Pa_ReadStream(stream, buffer, num_samples);    
}


void PortaudioStream::writeToStream(float* buffer, int num_samples)
{
    Pa_WriteStream(stream, buffer, num_samples);
}
