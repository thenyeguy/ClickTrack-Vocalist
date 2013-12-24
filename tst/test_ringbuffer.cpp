#include <iostream>
#include "../src/ringbuffer.h"

using namespace std;
using namespace ClickTrackUtils;


int main()
{
    RingBuffer<short> ring(4);
    cout << "Starting test..." << "\n\n" << endl;


    // Test get first element
    try
    {
        ring.get(0);
        cerr << "Failed to throw exception on empty buffer" << endl;
    }
    catch(RingBufferOutOfRange&)
    {
        cout << "Caught empty buffer." << endl;
    }


    // Add some elements
    ring.add(0);
    ring.add(1);
    ring.add(2);
    ring.add(3);


    // Try to view elements
    for(unsigned t = 0; t < 4; t++)
    {
        cout << "Time t=" << t << ": " << ring.get(t) << endl;
        if(ring.get(t) != t)
            throw "Failed test on proper lookup";
    }


    // Try to add first overlap element
    ring.add(4);
    cout << "Time t=" << 4 << ": " << ring.get(4) << endl;
    if(ring.get(4) != 4)
        throw "Failed test on proper lookup";


    // Test under and overflow of buffer
    try
    {
        ring.get(0);
        throw "Failed to throw exception on underflow of buffer";
    }
    catch(RingBufferOutOfRange&)
    {
        cout << "Caught underflow of buffer." << endl;
    }

    try
    {
        ring.get(5);
        throw "Failed to throw exception on overflow of buffer";
    }
    catch(RingBufferOutOfRange&)
    {
        cout << "Caught overflow of buffer." << endl;
    }


    // Test get range
    short buffer[3];
    ring.get_range(buffer, 1, 4);

    cout << "\n" << "Getting range:" << endl;
    for(unsigned i = 0; i < 3; i++)
    {
        cout << "Time t=" << i+1 << ": " << buffer[i] << endl;
        if(buffer[i] != i+1)
            throw "Failed test on proper lookup";
    }


    // Test overwrite
    ring[2] = 10;
    ring.get_range(buffer, 1, 4);
    cout << "\n" << "Getting range:" << endl;
    for(unsigned i = 0; i < 3; i++)
    {
        cout << "Time t=" << i+1 << ": " << buffer[i] << endl;
    }


    cout << "\n\n" << "All tests passed!" << endl;

    return 0;
}
