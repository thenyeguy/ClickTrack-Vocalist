#include <iostream>
#include "ringbuffer.h"

using namespace std;
using namespace ClickTrackUtils;


int main()
{
    RingBuffer<short> ring(4);
    cout << "Starting test..." << "\n\n" << endl;


    // Test get first element
    try
    {
        ring.get_sample(0);
        cerr << "Failed to throw exception on empty buffer" << endl;
    }
    catch(RingBufferOutOfRange&)
    {
        cout << "Caught empty buffer." << endl;
    }


    // Add some elements
    ring.add_sample(0);
    ring.add_sample(1);
    ring.add_sample(2);
    ring.add_sample(3);


    // Try to view elements
    for(unsigned t = 0; t < 4; t++)
    {
        cout << "Time t=" << t << ": " << ring.get_sample(t) << endl;
        if(ring.get_sample(t) != t)
            throw "Failed test on proper lookup";
    }


    // Try to add first overlap element
    ring.add_sample(4);
    cout << "Time t=" << 4 << ": " << ring.get_sample(4) << endl;
    if(ring.get_sample(4) != 4)
        throw "Failed test on proper lookup";


    // Test under and overflow of buffer
    try
    {
        ring.get_sample(0);
        throw "Failed to throw exception on underflow of buffer";
    }
    catch(RingBufferOutOfRange&)
    {
        cout << "Caught underflow of buffer." << endl;
    }

    try
    {
        ring.get_sample(5);
        throw "Failed to throw exception on overflow of buffer";
    }
    catch(RingBufferOutOfRange&)
    {
        cout << "Caught overflow of buffer." << endl;
    }

    cout << "\n\n" << "All tests passed!" << endl;

    return 0;
}
