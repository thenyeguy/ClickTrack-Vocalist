#include <iostream>
#include "../src/filter_generics.h"
#include "../src/io_elements.h"
#include "../src/oscillator.h"

using namespace std;
using namespace Portaudio;
using namespace IOElements;
using namespace Oscillators;



int main()
{
    try
    {
        cout << "Initializing signal chain" << endl;
        WavReader wav("wav/test.wav");

        Speaker speaker;
        speaker.set_input_channel(wav.get_output_channel(0),0);
        speaker.set_input_channel(wav.get_output_channel(1),1);

        WavWriter write("wav/out.wav");
        write.set_input_channel(wav.get_output_channel(0),0);
        write.set_input_channel(wav.get_output_channel(1),1);


        cout << "Entering process loop" << endl;
        while(!wav.is_done())
        {
            speaker.consume_inputs();
            write.consume_inputs();
        }
        cout << "Exiting" << "\n" << endl;
    }
    catch(exception& e)
    {
        cerr << "\n\n" << "EXCEPTION: " << e.what() << endl;
        exit(1);
    }
}
