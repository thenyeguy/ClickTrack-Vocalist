#include <iostream>
#include "../src/io_elements.h"

using namespace ClickTrack;


int main()
{
    try
    {
        std::cout << "Initializing signal chain" << std::endl;
        WavReader wav("wav/test.wav");

        Speaker speaker;
        speaker.set_input_channel(wav.get_output_channel(0),0);
        speaker.set_input_channel(wav.get_output_channel(1),1);

        WavWriter write("wav/out.wav");
        write.set_input_channel(wav.get_output_channel(0),0);
        write.set_input_channel(wav.get_output_channel(1),1);


        std::cout << "Entering process loop" << std::endl;
        while(!wav.is_done())
        {
            speaker.consume_inputs();
            write.consume_inputs();
        }
        std::cout << "Exiting" << "\n" << std::endl;
    }
    catch(std::exception& e)
    {
        std::cerr << "\n\n" << "EXCEPTION: " << e.what() << std::endl;
        exit(1);
    }
}
