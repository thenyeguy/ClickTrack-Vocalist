#ifndef PORTAUDIO_WRAPPER_H
#define PORTAUDIO_WRAPPER_H
#include <portaudio.h>


/* Define some key constants that can be used throughout the program.
 */
#define PA_DEFAULT_SAMPLE_RATE 44100 //hz
#define PA_DEFAULT_BUFFER_SIZE 1024  //slightly more than 20 ms


/* Define the constants for portaudio sampling.
 * Use 32-bit floating point samples.
 */
typedef float SAMPLE;
#define PA_SAMPLE_TYPE paFloat32
#define SAMPLE_SILENCE (0.0)
#define PRINTF_S_FORMAT "%1.4f"



/* A wrapper for the portaudio boilerplate code. Should initialize and close the
 * streams for us, and provide the ability to read from or write to the audio
 * stream.
 */
class PortaudioStream {
    private:
        PaStream* stream;

    public:
        /* Constructor and destructor automatically open and close the portaudio
         * streams for us. Uses default sample rate and buffer size
         */
        PortaudioStream();
        ~PortaudioStream();

        /* Given the location to store our data and the number of samples, reads
         * samples into a buffer.
         */
        void readFromStream(float*, int);

        /* Given a buffer and number of samples, writes those samples to our
         * output buffer.
         */
        void writeToStream(float*, int);
};


#endif
