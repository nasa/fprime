/**
 * IntervalTimerCommon.cpp:
 *
 * Contains the common functions for interval timer. This set of functions makes no assumption on
 * the format of the RawTime objects and thus it operates through functions that abstract that
 * implementation away, or it is working on the raw values, as raw values.
 *
 * *Note:* If the RawTime object is using U32 upper to store seconds and U32 lower to store nano
 * seconds, then X86/IntervalTimer.cpp can be used, and the implementor need only fill in the
 * getRawTime function for the specific OS.
 */

#include <Os/IntervalTimer.hpp>

#include <string.h>

namespace Os {

    IntervalTimer::IntervalTimer() {
        memset(&this->m_startTime,0,sizeof(this->m_startTime));
        memset(&this->m_stopTime,0,sizeof(this->m_stopTime));
    }

    IntervalTimer::~IntervalTimer() {}
    
    void IntervalTimer::start() {
        getRawTime(this->m_startTime);
    }
    
    void IntervalTimer::stop() {
        getRawTime(this->m_stopTime);
    }

    IntervalTimer::RawTime IntervalTimer::getDiffRaw(void) {
        return getDiffRaw(this->m_stopTime,this->m_stopTime);
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

    IntervalTimer::RawTime IntervalTimer::getSumRaw(const RawTime& t1, const RawTime& t2) {
        RawTime sum;
        const U64 al = (((U64) t1.upper << 32) + (U64) t1.lower);
        const U64 bl = (((U64) t2.upper << 32) + (U64) t2.lower);
        sum.lower = (U32) (al + bl);
        sum.upper = (U32) ((U64)(al + bl) >> 32);
        return sum;
    }

    U32 IntervalTimer::getDiffUsec(void) {
        return getDiffUsec(this->m_stopTime, this->m_startTime);
    }
}
