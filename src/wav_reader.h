#ifndef WAV_READER_H
#define WAV_READER_H

#include <exception>
#include <fstream>
#include <string>
#include "audio_generics.h"
#include "portaudio_wrapper.h"


namespace ClickTrack
{
    /* The WavReader is an input device. It reads a wav file and plays it back
     * in stereo until the file runs out. Then it stops forever.
     */
    class WavReader : public AudioGenerator
    {
        public:
            WavReader(const char* in_filename);

            /* Returns true once we have read out the entire wav file.
             */ 
            bool is_done();

            /* Resets the WavReader to point to the head of the file.
             */ 
            void restart();

            /* Returns the number of samples in the wav file
             */
            unsigned get_total_samples();

        private:
            void generate_outputs(std::vector<SAMPLE>& output, unsigned long t);


            const char* filename;
            std::ifstream file;

            bool stereo; //true iff stereo audio
            unsigned short byte_depth; // bytes per sample

            unsigned samples_total; // total samples
            unsigned samples_read;
    };


    /* Thrown when we can't correctly parse an given wav file
     */
    class InvalidWavFile: public std::exception
    {
        public:
            InvalidWavFile(const char* in_error)
                : error(in_error) {}

            virtual const char* what() const throw()
            {
                return error;
            }

        private:
            const char* error;
    };
}


#endif
