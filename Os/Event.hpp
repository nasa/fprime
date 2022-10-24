// File: Event.hpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov)
// Date: 27 July, 2018
//
// OS-independent wrapper for events.

#ifndef EVENT_HPP
#define EVENT_HPP

#include <FpConfig.hpp>
#include <Os/TaskId.hpp>

namespace Os {
    class Event {
        public:
            enum Timeout {
                EV_NO_WAIT      = 0,
                EV_WAIT_FOREVER = -1,
            };

            enum Options {
                EV_EVENTS_WAIT_ALL      = 0x0U,
                EV_EVENTS_WAIT_ANY      = 0x1U,
                EV_EVENTS_RETURN_ALL    = 0x2U,
                EV_EVENTS_KEEP_UNWANTED = 0x4U,
                EV_EVENTS_FETCH         = 0x80U,
            };

            static I32 send(const TaskId& tid, const U32 events);
            static I32 receive(const U32 events, const U8 options,
                               const I32 timeout, U32* eventsReceived);
            static I32 clear();
    };
}

#endif
