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

    void IntervalTimer::start() {
        this->m_startTime.getRawTime();
    }

    void IntervalTimer::stop() {
        this->m_stopTime.getRawTime();
    }

    U32 IntervalTimer::getDiffUsec() const {
        U32 result = 0;
        Os::RawTime::Status status = this->m_stopTime.getDiffUsec(this->m_startTime, result);
        if (status == Os::RawTime::Status::OP_OVERFLOW) {
            // If the operation fails due to overflow, we return the max value
            result = std::numeric_limits<U32>::max();
        }
        return result;
    }

    Fw::TimeInterval IntervalTimer::getTimeInterval() const {
        Fw::TimeInterval interval;
        Os::RawTime::Status status = this->m_stopTime.getTimeInterval(this->m_startTime, interval);
        if (status != Os::RawTime::Status::OP_OK) {
            // TODO: better error handling here
            // If the interval is too large to fit in the TimeInterval, we return a max value
            interval.set(std::numeric_limits<U32>::max(), std::numeric_limits<U32>::max());
        }
        return interval;
    }
}
