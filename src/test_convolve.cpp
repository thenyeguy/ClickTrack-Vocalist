#include <iostream>
#include "io_elements.h"
#include "oscillator.h"
#include "elementary_filters.h"
#include "convolve.h"

using namespace FilterGenerics;
using namespace IOElements;
using namespace Oscillators;
using namespace Filters;


int main()
{
    try
    {
        TriangleWave t(440.0f); 
        OutputChannel* tout[1] = {t.get_output_channel()};
        GainFilter g(0.4f, tout, 1);

        SAMPLE impulse[2] = {1,1};
        ConvolutionFilter f(g.get_output_channel(), 2, impulse);

        OutputChannel* out[2] = {g.get_output_channel(), f.get_output_channel()};
        Speaker s(out, 2);
        WavWriter w("convolve.wav", out, 2);


        w.consume_inputs();
        s.consume_inputs();
        w.consume_inputs();
        s.consume_inputs();
        w.consume_inputs();
        s.consume_inputs();
        w.consume_inputs();
        s.consume_inputs();
    }
    catch(exception& e)
    {
        cerr << "\n\n" << "EXCEPTION: " << typeid(e).name() << endl;
        cerr << "           " << e.what() << endl;

        exit(1);
    }

    return 0;
}
