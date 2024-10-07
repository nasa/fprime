// TODO: document
/**
 * IntervalTimerCommon.cpp:
 *
 * Contains the common functions for interval timer. This set of functions makes no assumption on
 * the format of the RawTime objects and thus it operates through functions that abstract that
 * implementation away, or it is working on the raw values, as raw values.
 *
 * *Note:* If the RawTime object is using U32 upper to store seconds and U32 lower to store nano
 * seconds, then X86/IntervalTimer.cpp can be used, and the implementer need only fill in the
 * getRawTime function for the specific OS.
 */

#include <Os/IntervalTimer.hpp>

#include <cstring>

namespace Os {

    IntervalTimer::IntervalTimer() : m_startTime(), m_stopTime() {}

    // IntervalTimer::~IntervalTimer() {}
    void IntervalTimer::getRawTime(RawTime& time) {
        time.getRawTime();
    }

    // use a Fw::TimeInterval (in Fw/Time), using U32; U32
    U32 IntervalTimer::getDiffUsec(const RawTime& t1, const RawTime& t2) {
        U32 result = 0;
        t1.getDiffUsec(t2, result);
        return result;
    }

    void IntervalTimer::start() {
        getRawTime(this->m_startTime);
    }

    void IntervalTimer::stop() {
        getRawTime(this->m_stopTime);
    }

    U32 IntervalTimer::getDiffUsec() {
        return getDiffUsec(this->m_stopTime, this->m_startTime);
    }
}
