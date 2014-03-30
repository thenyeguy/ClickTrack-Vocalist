#ifndef PORTAUDIO_WRAPPER_H
#define PORTAUDIO_WRAPPER_H

#include <portaudio.h>
#include <vector>


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
     * Currently, a buffer size of 128 is the lowest power of two that will run
     * skipfree on my laptop.
     */
    const unsigned SAMPLE_RATE = 44100; //hz
    const unsigned BUFFER_SIZE = 256;


    /* A wrapper for the portaudio boilerplate code. Should initialize and close
     * the streams for us, and provide the ability to read from an audio stream.
     */
    class InputStream {
        public:
            /* Constructor and destructor automatically open and close the
             * portaudio streams for us. Uses default sample rate and buffer
             * size.
             *
             * If useDefault is false, then a chooser is presented to the user
             */
            InputStream(unsigned in_channels = 1, bool useDefault=true);
            ~InputStream();

            /* Given a reference to a vector of channel data, reads the data
             * from a stream.
             */
            void readFromStream(std::vector< std::vector<SAMPLE> >& out);

        private:
            PaStream* stream;
            const unsigned channels;
            SAMPLE* buffer;
    };


    /* A wrapper for the portaudio boilerplate code. Should initialize and close
     * the streams for us, and provide the ability to write to the audio stream.
     */
    class OutputStream {
        public:
            /* Constructor and destructor automatically open and close the
             * portaudio streams for us. Uses default sample rate and buffer
             * size.
             *
             * If useDefault is false, then a chooser is presented to the user
             */
            OutputStream(unsigned in_channels = 1, bool useDefault=true);
            ~OutputStream();

            /* Given a reference to a vector of channel data, writes the data to
             * a stream.
             */
            void writeToStream(std::vector< std::vector<SAMPLE> >& in);

        private:
            PaStream* stream;
            const unsigned channels;
            SAMPLE* buffer;
    };
}


#endif
