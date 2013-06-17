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



namespace Portaudio {
    /* Define some key constants that can be used throughout the program.
    */
    const unsigned DEFAULT_SAMPLE_RATE = 44100; //hz
    const unsigned DEFAULT_BUFFER_SIZE = 256;


    /* A wrapper for the portaudio boilerplate code. Should initialize and close the
     * streams for us, and provide the ability to read from an audio stream.
     */
    class InputStream {
        private:
            PaStream* stream;

        public:
            /* Constructor and destructor automatically open and close the portaudio
             * streams for us. Uses default sample rate and buffer size
             */
            InputStream();
            ~InputStream();

            /* Given the location to store our data and the number of samples, reads
             * samples into a buffer.
             */
            void readFromStream(SAMPLE*, int);
    };


    /* A wrapper for the portaudio boilerplate code. Should initialize and close the
     * streams for us, and provide the ability to write to the audio stream.
     */
    class OutputStream {
        private:
            PaStream* stream;

        public:
            /* Constructor and destructor automatically open and close the portaudio
             * streams for us. Uses default sample rate and buffer size
             */
            OutputStream();
            ~OutputStream();

            /* Given a buffer and number of samples, writes those samples to our
             * output buffer.
             */
            void writeToStream(SAMPLE*, int);
    };
}


#endif
