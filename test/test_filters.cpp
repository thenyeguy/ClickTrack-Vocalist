#include <iostream>
#include "../src/io_elements.h"
#include "../src/oscillator.h"
#include "../src/elementary_filters.h"
#include "../src/filters.h"

using namespace ClickTrack;


int main()
{
    std::cout << "Initializing signal chain" << std::endl;
    SawWave saw(440.f);
    GainFilter saw_gain(0.7);
    saw_gain.set_input_channel(saw.get_output_channel());

    PassFilter pass(PassFilter::low, 5000);
    pass.set_input_channel(saw_gain.get_output_channel());

    Speaker speaker;
    speaker.set_input_channel(pass.get_output_channel());


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
