#ifndef SCHEDULER_CPP
#define SCHEDULER_CPP

#include "scheduler.h"

using namespace ClickTrack;


template<typename scheduledClass>
FunctionScheduler<scheduledClass>::FunctionScheduler(scheduledClass& in_caller)
    : caller(in_caller), event_i(0), events() {}

template<typename scheduledClass>
void FunctionScheduler<scheduledClass>::schedule(unsigned long time, 
        callback_t f, void* payload)
{
    // Push a tuple to the back
    events.push( {event_i, time, f, payload} );
    event_i++;
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

    // Return
    return eventsTriggered;
}

#endif
