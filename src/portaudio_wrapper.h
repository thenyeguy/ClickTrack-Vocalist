#ifndef PORTAUDIO_WRAPPER_H
#define PORTAUDIO_WRAPPER_H
#include <portaudio.h>


/* Define the constants for portaudio library.
 * Use 32-bit floating point samples.
 */
typedef float SAMPLE;
#define PA_SAMPLE_TYPE paFloat32
#define SAMPLE_SILENCE (0.0)
#define PRINTF_S_FORMAT "%1.4f"



namespace ClickTrack
{
    /* Define some key constants that can be used throughout the program.
     *
     * Currently, a buffer size of 128 is the lowest power of two that will
     * run skipfree on my laptop.
     */
    const unsigned SAMPLE_RATE = 44100; //hz
    const unsigned PORTAUDIO_BUFFER_SIZE = 128;


    /* A wrapper for the portaudio boilerplate code. Should initialize and close the
     * streams for us, and provide the ability to read from an audio stream.
     */
    class InputStream {
        public:
            /* Constructor and destructor automatically open and close the portaudio
             * streams for us. Uses default sample rate and buffer size
             */
            InputStream(unsigned in_channels = 1);
            ~InputStream();

            /* Given the location to store our data and the number of samples, reads
             * samples into a buffer.
             */
            void readFromStream(SAMPLE*, int);

        private:
            PaStream* stream;
            const unsigned channels;
    };


    /* A wrapper for the portaudio boilerplate code. Should initialize and close the
     * streams for us, and provide the ability to write to the audio stream.
     */
    class OutputStream {
        public:
            /* Constructor and destructor automatically open and close the portaudio
             * streams for us. Uses default sample rate and buffer size
             */
            OutputStream(unsigned in_channels = 1);
            ~OutputStream();

            /* Given a list of buffers and number of samples, writes those samples to our
             * output buffer.
             */
            void writeToStream(SAMPLE*, int);

        private:
            PaStream* stream;
            const unsigned channels;
    };
}


#endif
