#include <iostream>
#include "../src/io_elements.h"
#include "../src/reverb.h"

using namespace ClickTrack;


int main()
{
    try
    {
        std::cout << "Establishing signal chain" << std::endl;
        Microphone mic;

        SimpleReverb rev(1.0, 0.2, 0.5, 1);
        rev.set_input_channel(mic.get_output_channel());

        Speaker out;
        out.set_input_channel(rev.get_output_channel());


        std::cout << "Playing" << std::endl;
        while(true)
            out.consume_inputs();

        std::cout << "Done" << std::endl;
    }
    catch(std::exception& e)
    {
        std::cerr << "\n\n" << "EXCEPTION: " << typeid(e).name() << std::endl;
        std::cerr << "           " << e.what() << std::endl;

        exit(1);
    }

    return 0;
}
