/**
 * X86/IntervalTimer.cpp:
 *
 * This file supports the core functions of the IntervalTimer for X86 implementations that support
 * the following specification for the "RawTime" object:
 *
 * RawTime.lower = nanoseconds of time
 * RawTime.upper = seconds of time.
 *
 * Any implementation that fills "RawTime" via this specification can use these basic implementations.
 *
 * Note: this file is cloned from the original Linux implementation.
 */
#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>

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

    U32 IntervalTimer::getDiffUsec(void) {
        return getDiffUsec(this->m_stopTime,this->m_startTime);
    }

    // Adapted from: http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html

    // should be t1In - t2In

    U32 IntervalTimer::getDiffUsec(const RawTime& t1In, const RawTime& t2In) {

        RawTime result = {t1In.upper - t2In.upper,0};

        if (t1In.lower < t2In.lower) {
            result.upper -= 1; // subtract nsec carry to seconds
            result.lower = t1In.lower + (1000000000 - t2In.lower);
        } else {
            result.lower = t1In.lower - t2In.lower;
        }

        return result.upper*1000000 + result.lower / 1000;
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
