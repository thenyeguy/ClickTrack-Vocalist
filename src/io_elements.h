#ifndef IOELEMENTS_H
#define IOELEMENTS_H

#include <exception>
#include <fstream>
#include <string>
#include "portaudio_wrapper.h"
#include "audio_generics.h"


namespace ClickTrack
{
    /* The microphone is an input device. It uses the default input device on
     * your computer, and pulls its data from portaudio.
     */
    class Microphone : public AudioGenerator
    {
        public:
            Microphone(unsigned num_channels = 1);

        private:
            void generate_outputs(std::vector< std::vector<SAMPLE> >& outputs);


            InputStream stream;
    };


    /* The speaker is an output device. It uses the default output device on
     * your computer, and pushes its data out to portaudio.
     */
    class Speaker : public AudioConsumer
    {
        public:
            Speaker(unsigned num_inputs = 1);

        private:
            void process_inputs(std::vector< std::vector<SAMPLE> >& inputs);


            OutputStream stream;
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
            void generate_outputs(std::vector< std::vector<SAMPLE> >& outputs);


            const char* filename;
            std::ifstream file;

            bool stereo; //true iff stereo audio
            unsigned short byte_depth; // bytes per sample

            unsigned samples_total; // total samples
            unsigned samples_read;
    };


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
            void process_inputs(std::vector< std::vector<SAMPLE> >& inputs);


            const char* filename;
            std::ofstream file;

            unsigned samples_written; // number of frames of samples
    };
}


#endif
