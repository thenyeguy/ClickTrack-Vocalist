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
        std::cout << "Establishing signal chain" << std::endl;

        Microphone mic;

        OutputChannel* revch[1] = {mic.get_output_channel()};
        SimpleReverb rev(1.0, 0.2, 0.5, 1, revch);

        OutputChannel* outch[1] = {rev.get_output_channel()};
        Speaker out(outch,1);

        std::cout << "Playing" << std::endl;
        while(true)
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
