#ifndef SCHEDULER_CPP
#define SCHEDULER_CPP

#include "scheduler.h"
#include <iostream>

using namespace ClickTrack;


template<typename scheduledClass>
FunctionScheduler<scheduledClass>::FunctionScheduler(scheduledClass& in_caller)
    : caller(in_caller), events() {}

template<typename scheduledClass>
void FunctionScheduler<scheduledClass>::schedule(unsigned long time, 
        callback_t f, void* payload)
{
    // Push a tuple to the back
    events.push( {time, f, payload} );
}


template<typename scheduledClass>
unsigned FunctionScheduler<scheduledClass>::run(unsigned long time)
{
    // Trigger all the found events
    unsigned eventsTriggered = 0;
    while(!events.empty() && events.top().t <= time)
    {
        // Get the event
        struct event_t event = events.top();
        events.pop();
        eventsTriggered++;

        // Call the event
        event.f(caller, event.payload);
    }

    // Erase this event from the map and return
    return eventsTriggered;
}

#endif
