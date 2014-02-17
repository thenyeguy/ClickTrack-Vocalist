#include <iostream>
#include "../src/io_elements.h"
#include "../src/oscillator.h"

using namespace ClickTrack;


int main()
{
    std::cout << "Initializing signal chain" << std::endl;

    TriangleWave blep(8*440.f);
    SimpleTriangleWave naive(8*440.f);

    Speaker speaker(2);
    speaker.set_input_channel(blep.get_output_channel(), 0);
    speaker.set_input_channel(naive.get_output_channel(), 1);

    WavWriter wav("wav/test_osc.wav", 2);
    wav.set_input_channel(blep.get_output_channel(), 0);
    wav.set_input_channel(naive.get_output_channel(), 1);


    // Loop small number of times to generate enough audio
    std::cout << "Entering process loop" << std::endl;
    for(unsigned i = 0; i < SAMPLE_RATE/FRAME_SIZE; i++)
    {
        try
        {
            speaker.consume_inputs();
            wav.consume_inputs();
        }
        catch(std::exception& e)
        {
            std::cerr << "\n\n" << "EXCEPTION: " << e.what() << std::endl;
            exit(1);
        }
    }
}
