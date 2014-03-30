#ifndef WAV_WRITER_H
#define WAV_WRITER_H

#include <fstream>
#include <string>
#include "audio_generics.h"
#include "portaudio_wrapper.h"


namespace ClickTrack
{
    /* The WavWriter is an output device. It listens on its input ports and
     * writes it out to a wav file.
     *
     * Must be terminated gracefully to update the length params in the header
     * Uses 16-bit integer PCM
     */
    class WavWriter : public AudioConsumer
    {
        public:
            WavWriter(const char* in_filename, unsigned num_inputs = 1);
            ~WavWriter();

        private:
            void process_inputs(std::vector<SAMPLE>& input, unsigned long t);


            const char* filename;
            std::ofstream file;

            unsigned samples_written; // number of frames of samples
    };
}


#endif
