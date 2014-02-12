#include <iostream>
#include "../src/io_elements.h"
#include "../src/oscillator.h"
#include "../src/elementary_filters.h"
#include "../src/delay.h"

using namespace ClickTrack;


int main()
{
    std::cout << "Initializing signal chain" << std::endl;
    Microphone mic;

    Delay delay(0.5, 0.5, 0.5);
    delay.set_input_channel(mic.get_output_channel());

    GainFilter mic_gain(0.5);
    mic_gain.set_input_channel(delay.get_output_channel());

    TriangleWave tri(440.f);
    GainFilter tri_gain(0.5);
    tri_gain.set_input_channel(tri.get_output_channel());

    Adder add(2);
    add.set_input_channel(mic_gain.get_output_channel(), 0);
    add.set_input_channel(tri_gain.get_output_channel(), 1);

    Speaker speaker(2);
    speaker.set_input_channel(mic_gain.get_output_channel(), 0);
    speaker.set_input_channel(tri_gain.get_output_channel(), 1);




    std::cout << "Entering process loop" << std::endl;
    while(true)
    {
        try
        {
            speaker.consume_inputs();
        }
        catch(std::exception& e)
        {
            std::cerr << "\n\n" << "EXCEPTION: " << e.what() << std::endl;
            exit(1);
        }
    }
}
