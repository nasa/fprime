#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>

#include <mach/clock.h>
#include <mach/mach.h>

namespace Os {
    
    IntervalTimer::IntervalTimer() {
        memset(&this->m_startTime,0,sizeof(this->m_startTime));
        memset(&this->m_stopTime,0,sizeof(this->m_stopTime));
    }

    IntervalTimer::~IntervalTimer() {
    }

    U32 IntervalTimer::getTimerFrequency(void) {
        return 1;
    }

    void IntervalTimer::getRawTime(RawTime& time) {

		clock_serv_t cclock;
		mach_timespec_t mts;
		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
		clock_get_time(cclock, &mts);
		mach_port_deallocate(mach_task_self(), cclock);
		time.upper = mts.tv_sec;
		time.lower = mts.tv_nsec;
    }

    U32 IntervalTimer::getDiffUsec(void) {
        return getDiffUsec(this->m_stopTime,this->m_startTime);
    }

    // Adapted from: http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html

    U32 IntervalTimer::getDiffUsec(const RawTime& t1In, const RawTime& t2In) {

        RawTime result;
        RawTime t1 = t1In;
        RawTime t2 = t2In;

        /* Perform the carry for the later subtraction by updating t2. */
        if (t1.lower < t2.lower) {
            int nsec = (t2.lower - t1.lower) / 1000000000 + 1;
            t2.lower -= 1000000 * nsec;
            t2.upper += nsec;
        }
        if (t1.lower - t2.lower > 1000000) {
            int nsec = (t1.lower - t2.lower) / 1000000000;
            t2.lower += 1000000 * nsec;
            t2.upper -= nsec;
        }

        result.upper = t1.upper - t2.upper;
        result.lower = t1.lower - t2.lower;

        return result.upper * 1000000 + result.lower / 1000;

    }

    void IntervalTimer::start() {
        getRawTime(this->m_startTime);
    }

    void IntervalTimer::stop() {
        getRawTime(this->m_stopTime);
    }

    IntervalTimer::RawTime IntervalTimer::getDiffRaw(const RawTime& t1, const RawTime& t2) {
        RawTime diff;
        const U64 al = (((U64) t1.upper << 32) + (U64) t1.lower);
        const U64 bl = (((U64) t2.upper << 32) + (U64) t2.lower);
        if (t2.lower > t1.lower) {
            diff.lower = 0xFFFFFFFF - (t2.lower - t1.lower - 1);
            diff.upper = t1.upper - t2.upper - 1;
        } else {
            diff.lower = (U32) (al - bl);
            diff.upper = (U32) ((U64)(al - bl) >> 32);
        }
        return diff;
    }
        
}


