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


InputStream::InputStream(unsigned in_channels, bool useDefault)
    : channels(in_channels)
{
    // Initialize portaudio
    pa_error_check("PaInitialize", Pa_Initialize());

    // If no device specified, ask the user for one
    PaDeviceIndex device = Pa_GetDefaultInputDevice();
    if(!useDefault)
    {
        // First list all the channels
        int nChannels = Pa_GetDeviceCount();
        if(nChannels < 0)
        {
            printf( "ERROR: Pa_CountDevices returned 0x%x\n", nChannels );
            PaError err = nChannels;
            std::cout << Pa_GetErrorText(err) << std::endl;
            exit(1);
        }
        std::cout << std::endl << "There are " << nChannels <<
            " audio devices available." << std::endl;
        for(int i=0; i < nChannels; i++)
        {
            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
            std::cout << "  Device #" << i << ": " <<
                deviceInfo->name << " (" << deviceInfo->maxInputChannels <<
                " in, " << deviceInfo->maxOutputChannels << " out)" << std::endl;
        }
                
        // Ask for a device
        while(true)
        {
            std::cout << "Choose an input device: ";
            std::cin >> device;

            if(!(0 <= device && device < nChannels))
                std::cerr << "    Not a valid channel number." << std::endl;
            else if(Pa_GetDeviceInfo(device)->maxInputChannels < channels)
                std::cerr << "    Not enough input channels." << std::endl;
            else
                break;
        }
    }

    // Set up the stream
    PaStreamParameters inputParams;
    inputParams.device = device;
    inputParams.channelCount = channels;
    inputParams.sampleFormat = PA_SAMPLE_TYPE;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    //Open the stream!
    pa_error_check("Pa_OpenStream",
        Pa_OpenStream(&stream, &inputParams, NULL,
            SAMPLE_RATE, BUFFER_SIZE, paNoFlag,
            NULL, NULL));
    pa_error_check("Pa_StartStream", Pa_StartStream(stream));

    // Initialize buffer for writing
    buffer = new SAMPLE[channels*BUFFER_SIZE];
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
    Pa_ReadStream(stream, buffer, BUFFER_SIZE);    

    // Deinterleave our results
    for(int i = 0; i < channels; i++)
    {
        for(int j = 0; j < BUFFER_SIZE; j++)
            out[i][j] = buffer[channels*j + i];
    }
}




OutputStream::OutputStream(unsigned in_channels, bool useDefault)
    : channels(in_channels)
{
    // Initialize portaudio
    pa_error_check("PaInitialize", Pa_Initialize());

    // If no device specified, ask the user for one
    PaDeviceIndex device = Pa_GetDefaultOutputDevice();
    if(!useDefault)
    {
        // First list all the channels
        int nChannels = Pa_GetDeviceCount();
        if(nChannels < 0)
        {
            printf( "ERROR: Pa_CountDevices returned 0x%x\n", nChannels );
            PaError err = nChannels;
            std::cout << Pa_GetErrorText(err) << std::endl;
            exit(1);
        }
        std::cout << std::endl << "There are " << nChannels <<
            " audio devices available." << std::endl;
        for(int i=0; i < nChannels; i++)
        {
            const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
            std::cout << "  Device #" << i << ": " <<
                deviceInfo->name << " (" << deviceInfo->maxInputChannels <<
                " in, " << deviceInfo->maxOutputChannels << " out)" << std::endl;
        }
                
        // Ask for a device
        while(true)
        {
            std::cout << "Choose an output device: ";
            std::cin >> device;

            if(!(0 <= device && device < nChannels))
                std::cerr << "    Not a valid channel number." << std::endl;
            else if(Pa_GetDeviceInfo(device)->maxOutputChannels < channels)
                std::cerr << "    Not enough input channels." << std::endl;
            else
                break;
        }
    }

    // Set up the stream
    PaStreamParameters outputParams;
    outputParams.device = device;
    outputParams.channelCount = channels;
    outputParams.sampleFormat = PA_SAMPLE_TYPE;
    outputParams.suggestedLatency =
        Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

    //Open the stream!
    pa_error_check("Pa_OpenStream",
        Pa_OpenStream(&stream, NULL, &outputParams,
            SAMPLE_RATE, BUFFER_SIZE, paNoFlag,
            NULL, NULL));
    pa_error_check("Pa_StartStream", Pa_StartStream(stream));

    // Initialize buffer for writing
    buffer = new SAMPLE[channels*BUFFER_SIZE];
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
        for(int j = 0; j < BUFFER_SIZE; j++)
        {
            SAMPLE sample = in[i][j];
            if(sample > 1.0) sample = 1.0;
            if(sample < -1.0) sample = -1.0;
            buffer[channels*j + i] = sample;
        }
    }

    // Write out to the stream
    Pa_WriteStream(stream, buffer, BUFFER_SIZE);
}
