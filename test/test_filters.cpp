#include <cstdlib>
#include <iostream>
#include "../src/first_order_filter.h"
#include "../src/oscillator.h"
#include "../src/second_order_filter.h"
#include "../src/wav_reader.h"
#include "../src/wav_writer.h"

using namespace ClickTrack;


int main()
{
    std::cout << "Initializing signal chain" << std::endl;
    WavReader impulse("wav/delta.wav");

    SecondOrderFilter filter(SecondOrderFilter::PEAK, 5000, -3, 10.0);
    filter.set_input_channel(impulse.get_output_channel());

    SecondOrderFilter filter2(SecondOrderFilter::PEAK, 15000, 3, 10.0);
    filter2.set_input_channel(filter.get_output_channel());

    WavWriter wav("wav/test_filters.wav");
    wav.set_input_channel(filter2.get_output_channel());


    std::cout << "Entering process loop" << std::endl;
    for(unsigned i = 0; i < 44100; i++)
    {
        try
        {
            wav.consume();
        }
        catch(std::exception& e)
        {
            std::cerr << "\n\n" << "EXCEPTION: " << e.what() << std::endl;
            exit(1);
        }
    }
    std::cout << "Complete" << std::endl;
}
