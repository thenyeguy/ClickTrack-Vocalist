#include <cstdlib>
#include <iostream>
#include "../src/oscillator.h"
#include "../src/speaker.h"
#include "../src/wav_writer.h"

using namespace ClickTrack;


int main()
{
    std::cout << "Initializing signal chain" << std::endl;

    Oscillator blep(Oscillator::BlepTri, 440);
    blep.set_transposition(12*3);
    Oscillator naive(Oscillator::Tri, 440);
    naive.set_transposition(12*3);

    Speaker speaker(2);
    speaker.set_input_channel(blep.get_output_channel(), 0);
    speaker.set_input_channel(naive.get_output_channel(), 1);

    WavWriter wav("wav/test_osc.wav", 2);
    wav.set_input_channel(blep.get_output_channel(), 0);
    wav.set_input_channel(naive.get_output_channel(), 1);


    // Loop small number of times to generate enough audio
    std::cout << "Entering process loop" << std::endl;
    for(unsigned i = 0; i < SAMPLE_RATE; i++)
    {
        try
        {
            speaker.consume();
            wav.consume();
        }
        catch(std::exception& e)
        {
            std::cerr << "\n\n" << "EXCEPTION: " << e.what() << std::endl;
            exit(1);
        }
    }
}
