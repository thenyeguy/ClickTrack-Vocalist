#include <iostream>
#include "../src/io_elements.h"
#include "../src/oscillator.h"
#include "../src/elementary_filters.h"
#include "../src/convolve.h"
#include "../src/reverb.h"

using namespace FilterGenerics;
using namespace IOElements;
using namespace Oscillators;
using namespace Filters;


int main()
{
    try
    {
        std::cout << "Reading in impulse" << std::endl;
        impulse_pair* imp = impulse_from_wav("wav/test_impulse.wav");

        std::cout << "Establishing signal chain" << std::endl;
        WavReader in("wav/test.wav");

        ConvolutionReverb revl(imp->num_samples, imp->left,
                               0.25, 0.5);
        revl.set_input_channel(in.get_output_channel());
        //Reverb revr(imp->num_samples, imp->right, 0.2, 0.5, in.get_output_channel(1));

        WavWriter out("wav/conv_out.wav");
        out.set_input_channel(revl.get_output_channel());

        std::cout << "Playing" << std::endl;
        while(!in.is_done())
            out.consume_inputs();
        std::cout << "Done" << std::endl;
    }
    catch(exception& e)
    {
        cerr << "\n\n" << "EXCEPTION: " << typeid(e).name() << endl;
        cerr << "           " << e.what() << endl;

        exit(1);
    }

    return 0;
}
