#ifndef IOBLOCK_H
#define IOBLOCK_H
#include "portaudio_wrapper.h"
#include "filterblock.h"


namespace FilterChain
{
    class Microphone : public AudioElement
    {
        private:
            Portaudio::InputStream* streams;
        public:
            Microphone(Portaudio::InputStream* inputs, unsigned num_channels = 1);

            using AudioElement::generate_output;
            void generate_output();
    };


    class Speaker : public OutputElement
    {
        private:
            Portaudio::OutputStream* streams;
        public:
            Speaker(Portaudio::OutputStream* outputs,
                    AudioElement* input_elements, unsigned* input_chans,
                    unsigned num_input_elements = 1);
            using OutputElement::output;
            void output(SAMPLE** inputs);
    };
}


#endif
