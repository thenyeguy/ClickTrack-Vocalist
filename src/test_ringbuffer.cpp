#include <iostream>
#include "ringbuffer.h"

using namespace std;
using namespace ringbuffer;


int main()
{
    RingBuffer<short> small(2);
    RingBuffer<short> big(10);
    cout << "Starting test..." << endl << endl;

    // Test dequeue from empty
    try
    {
        small.deq();
        cerr << "Failed to throw exception on empty buffer" << endl;
    }
    catch(...)
    {
        cout << "Caught empty buffer." << endl;
    }


    // Test enqueue to full
    small.enq(0);
    small.enq(1);
    try
    {
        small.enq(2);
        cerr << "Failed to throw exception on full buffer" << endl;
    }
    catch(...)
    {
        cout << "Caught full buffer." << endl;
    }


    // Test normal
    big.enq(0);
    big.enq(1);
    big.enq(2);
    big.enq(3);
    big.enq(4);
    big.enq(5);
    big.enq(6);
    big.enq(7);
    big.enq(8);

    while(!big.is_empty())
    {
        short temp = big.deq();
        cout << "Got out: " << temp << endl;
    }


    // Test wrap around
    big.enq(9);
    big.enq(10);
    big.enq(11);
    big.enq(12);

    while(!big.is_empty())
    {
        short temp = big.deq();
        cout << "Got out: " << temp << endl;
    }

    return 0;
}
