#include <iostream>
#include "../src/fft.h"

using namespace std;
using namespace ClickTrack;


int main()
{
    Transformer t(8);
    
    float in_float[8] = {1,2,3,4,5,6,7,8};
    complex<float> in[8];
    for(int i = 0; i < 8; i++)
        in[i] = complex<float>(in_float[i]);

    complex<float> out[8];
    t.fft(in,out);

    for(int i = 0; i < 8; i++)
        cout << out[i] << endl;
    cout << endl;

    t.ifft(out, in);
    
    for(int i = 0; i < 8; i++)
        cout << in_float[i] << endl;
    cout << endl;

    return 0;
}
