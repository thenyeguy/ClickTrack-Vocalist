#include <iostream>
#include "io_elements.h"
#include "oscillator.h"
#include "elementary_filters.h"
#include "convolve.h"
#include "reverb.h"

using namespace FilterGenerics;
using namespace IOElements;
using namespace Oscillators;
using namespace Filters;


int main()
{
    try
    {
        /*
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
        */

        std::cout << "Reading in impulse" << std::endl;
        impulse_pair* imp = impulse_from_wav("wav/test_impulse.wav");

        std::cout << "Establishing signal chain" << std::endl;
        WavReader in("wav/test.wav");

        Reverb revl(imp->num_samples, imp->left, 0.2, 0.5, in.get_output_channel(0));
        //Reverb revr(imp->num_samples, imp->right, 0.2, 0.5, in.get_output_channel(1));
        //ConvolutionFilter f(in.get_output_channel(0), imp->num_samples, imp->left);

        OutputChannel* outch[1] = {revl.get_output_channel()};
        //Speaker out(outch, 2);
        WavWriter out("wav/conv_out.wav", outch, 1);

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
