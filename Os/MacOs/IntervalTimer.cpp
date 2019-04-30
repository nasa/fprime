/**
 * MacOs/IntervalTimer.cpp:
 *
 * The OSX implementation of the interval timer shares the same raw setup as other X86
 * implementations. That is: the lower U32 of the RawTime is nano-seconds, and the upper U32 of
 * RawTime object is seconds. Thus only the "getRawTime" function differes from the base X86
 * version of this file.
 */
#include <Os/IntervalTimer.hpp>

#include <mach/clock.h>
#include <mach/mach.h>

namespace Os {

    void IntervalTimer::getRawTime(RawTime& time) {
		clock_serv_t cclock;
		mach_timespec_t mts;
		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
		clock_get_time(cclock, &mts);
		mach_port_deallocate(mach_task_self(), cclock);
		time.upper = mts.tv_sec;
		time.lower = mts.tv_nsec;
    }
}
