#ifndef IOELEMENTS_H
#define IOELEMENTS_H

#include <exception>
#include <fstream>
#include <string>
#include "portaudio_wrapper.h"
#include "filter_generics.h"


namespace IOElements
{
    /* The microphone is an input device. It uses the default input device on
     * your computer, and pulls its data from portaudio.
     */
    class Microphone : public FilterGenerics::AudioGenerator
    {
        private:
            Portaudio::InputStream stream;
        public:
            Microphone(unsigned num_channels = 1);

            void generate_outputs(SAMPLE** outputs);
    };


    /* The speaker is an output device. It uses the default output device on
     * your computer, and pushes its data out to portaudio.
     */
    class Speaker : public FilterGenerics::AudioConsumer
    {
        private:
            Portaudio::OutputStream stream;
        public:
            Speaker(FilterGenerics::OutputChannel** inputs,
                    unsigned num_inputs = 1);

            void process_inputs(SAMPLE** inputs);
    };


    /* Thrown when we can't correctly parse an given wav file
     */
    class InvalidWavFile: public std::exception
    {
        private:
            const char* error;
        public:
            InvalidWavFile(const char* in_error)
                : error(in_error) {}

            virtual const char* what() const throw()
            {
                return error;
            }
    };


    /* The WavReader is an input device. It reads a wav file and plays it back
     * in stereo until the file runs out. Then it stops forever.
     */
    class WavReader : public FilterGenerics::AudioGenerator
    {
        private:
            const char* filename;
            std::ifstream file;

            bool stereo; //true iff stereo audio
            unsigned short byte_depth; // bytes per sample

            unsigned samples_total; // total samples
            unsigned samples_read;
        public:
            WavReader(const char* in_filename);

            void generate_outputs(SAMPLE** outputs);
    };
}


#endif
