// File: Event.cpp
// Author: Nathan Serafin (nathan.serafin@jpl.nasa.gov>
// Date: 27 July, 2018
//
// Linux (non-) implementation of events. Events are a
// feature of VxWorks; if a user wants them on Linux,
// they will need to provide an implementation here.

#include <Fw/Types/Assert.hpp>

#include <Os/Event.hpp>

namespace Os
{
    I32 Event::send(const TaskId& tid, const U32 events)
    {
        FW_ASSERT(0);
        return 0;
    }

    I32 Event::receive(const U32 events, const U8 options,
                       const I32 timeout, U32* eventsReceived)
    {
        FW_ASSERT(0);
        return 0;
    }

    I32 Event::clear(void)
    {
        FW_ASSERT(0);
        return 0;
    }
}
