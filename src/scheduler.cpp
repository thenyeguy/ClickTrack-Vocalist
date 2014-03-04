#ifndef SCHEDULER_CPP
#define SCHEDULER_CPP

#include "scheduler.h"

using namespace ClickTrack;


template<typename scheduledClass>
FunctionScheduler<scheduledClass>::FunctionScheduler(scheduledClass& in_caller)
    : caller(in_caller), events() {}

template<typename scheduledClass>
void FunctionScheduler<scheduledClass>::schedule(unsigned long time, 
        callback_t f, void* payload)
{
    // Push a tuple to the back
    events[time].push_back( {f, payload} );
}

template<typename scheduledClass>
unsigned FunctionScheduler<scheduledClass>::run(unsigned long time)
{
    // Get the list and stop if it doesn't exist
    auto eventList = events.find(time);
    if(eventList == events.end())
        return 0;

    // Trigger all the found events
    unsigned eventsTriggered = 0;
    for(auto event : eventList->second)
    {
        // Call the event
        event.f(caller, event.payload);
        eventsTriggered++;
    }

    // Erase this event from the map and return
    events.erase(eventList);
    return eventsTriggered;
}

#endif
