#include <iostream>
#include "../src/ringbuffer.h"

using namespace ClickTrack;


int main()
{
    RingBuffer<short> ring(4);
    std::cout << "Starting test..." << "\n\n" << std::endl;


    // Test get first element
    try
    {
        ring.get(0);
        std::cerr << "Failed to throw exception on empty buffer" << std::endl;
    }
    catch(RingBufferOutOfRange&)
    {
        std::cout << "Caught empty buffer." << std::endl;
    }


    // Add some elements
    ring.add(0);
    ring.add(1);
    ring.add(2);
    ring.add(3);


    // Try to view elements
    for(unsigned t = 0; t < 4; t++)
    {
        std::cout << "Time t=" << t << ": " << ring.get(t) << std::endl;
        if(ring.get(t) != t)
            throw "Failed test on proper lookup";
    }


    // Try to add first overlap element
    ring.add(4);
    std::cout << "Time t=" << 4 << ": " << ring.get(4) << std::endl;
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
        std::cout << "Caught underflow of buffer." << std::endl;
    }

    try
    {
        ring.get(5);
        throw "Failed to throw exception on overflow of buffer";
    }
    catch(RingBufferOutOfRange&)
    {
        std::cout << "Caught overflow of buffer." << std::endl;
    }


    // Test get range
    std::vector<short> buffer(3);
    ring.get_range(buffer, 1, 4);

    std::cout << "\n" << "Getting range:" << std::endl;
    for(unsigned i = 0; i < 3; i++)
    {
        std::cout << "Time t=" << i+1 << ": " << buffer[i] << std::endl;
        if(buffer[i] != i+1)
            throw "Failed test on proper lookup";
    }


    // Test overwrite
    ring[2] = 10;
    ring.get_range(buffer, 1, 4);
    std::cout << "\n" << "Getting range:" << std::endl;
    for(unsigned i = 0; i < 3; i++)
    {
        std::cout << "Time t=" << i+1 << ": " << buffer[i] << std::endl;
    }


    std::cout << "\n\n" << "All tests passed!" << std::endl;

    return 0;
}
