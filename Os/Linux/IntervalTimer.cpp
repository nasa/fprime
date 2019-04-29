/**
 * Linux/IntervalTimer.cpp:
 *
 * The Linux implementation of the interval timer shares the same raw setup as other X86
 * implementations. That is: the lower U32 of the RawTime is nano-seconds, and the upper U32 of
 * RawTime object is seconds. Thus only the "getRawTime" function differes from the base X86
 * version of this file.
 */
#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>
#include <time.h>
#include <errno.h>

namespace Os {
    void IntervalTimer::getRawTime(RawTime& time) {
        timespec t;

        FW_ASSERT(clock_gettime(CLOCK_REALTIME,&t) == 0,errno);
        time.upper = t.tv_sec;
        time.lower = t.tv_nsec;
    }
}
