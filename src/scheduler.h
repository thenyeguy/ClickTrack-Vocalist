#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>


namespace ClickTrack
{
    /* A parameter scheduler is meant to be used to handle scheduling of
     * parameter changes within classes at designated sample times. As
     * a templated class, you may require more than one if you have more than
     * one type for your parameters
     */
    template <class scheduledClass>
    class FunctionScheduler
    {
        public:
            FunctionScheduler(scheduledClass& in_caller);

            /* The function scheduler requires a callback function of the
             * following arguments:
             *     2. a reference to the calling object
             *     3. a void* payload that will be passed back to the callbak
             */
            typedef void (*callback_t)(scheduledClass& caller, void* payload);

            /* To schedule an event, you must pass the time of the event,
             * a reference to the object caller, the function being called, and
             * the value to change it to.
             */
            void schedule(unsigned long time, callback_t f, void* payload);

            /* Run must be called within the processing loop of your target
             * class at every sample time. It takes the time to process for, and
             * if any events are scheduled for that time, it triggers that
             * event.
             *
             * Returns the number of events processed
             */
            unsigned run(unsigned long time);

        private:
            /* Store the calling class so we can trigger its events
             */
            scheduledClass& caller;

            /* Internally, we map sample timestamps to their time, function,
             * and payload to be triggered. Events are ordered by their time and
             * stored in a priority queue. Attach an index to ensure first in
             * first out ordering
             */
            unsigned event_i;
            struct event_t { unsigned i; unsigned long t; callback_t f; 
                void* payload; };
            struct event_t_comp
            { 
                bool operator()(const struct event_t e1, const struct event_t e2)
                {
                    if(e1.t == e2.t)
                        return e1.i > e2.i;
                    return e1.t > e2.t;
                }
            };

            std::priority_queue<struct event_t, std::vector<struct event_t>,
                struct event_t_comp> events;
    };
}

#include "scheduler.cpp"

#endif
