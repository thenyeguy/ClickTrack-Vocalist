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
    // Initialize buffer for writing
    buffer = new SAMPLE[channels*PORTAUDIO_BUFFER_SIZE];
    
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
    // Free the buffer
    delete buffer;

    // Close portaudio
    pa_error_check("Pa_StopStream", Pa_StopStream(stream));
    pa_error_check("Pa_CloseStream", Pa_CloseStream(stream));
    pa_error_check("Pa_Terminate", Pa_Terminate());
}


void InputStream::readFromStream(std::vector< std::vector<SAMPLE> >& out)
{
    // Read in from the sream
    Pa_ReadStream(stream, buffer, PORTAUDIO_BUFFER_SIZE);    

    // Deinterleave our results
    for(int i = 0; i < channels; i++)
    {
        for(int j = 0; j < PORTAUDIO_BUFFER_SIZE; j++)
            out[i][j] = buffer[channels*j + i];
    }
}




OutputStream::OutputStream(unsigned in_channels)
    : channels(in_channels)
{
    // Initialize buffer for writing
    buffer = new SAMPLE[channels*PORTAUDIO_BUFFER_SIZE];
    
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
    // Free the buffer
    delete buffer;

    // Close portaudio
    pa_error_check("Pa_StopStream", Pa_StopStream(stream));
    pa_error_check("Pa_CloseStream", Pa_CloseStream(stream));
    pa_error_check("Pa_Terminate", Pa_Terminate());
}


void OutputStream::writeToStream(std::vector< std::vector<SAMPLE> >& in)
{
    // Interleave channels
    for(int i = 0; i < channels; i++)
    {
        for(int j = 0; j < PORTAUDIO_BUFFER_SIZE; j++)
        {
            SAMPLE sample = in[i][j];
            if(sample > 1.0) sample = 1.0;
            if(sample < -1.0) sample = -1.0;
            buffer[channels*j + i] = sample;
        }
    }

    // Write out to the stream
    Pa_WriteStream(stream, buffer, PORTAUDIO_BUFFER_SIZE);
}
