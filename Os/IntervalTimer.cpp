// ======================================================================
// \title Os/IntervalTimer.cpp
// \brief Implementation for Os::IntervalTimer
// ======================================================================

#include <Os/IntervalTimer.hpp>
#include <cstring>

namespace Os {

    IntervalTimer::IntervalTimer() : m_startTime(), m_stopTime() {}

    void IntervalTimer::start() {
        this->m_startTime.now();
    }

    void IntervalTimer::stop() {
        this->m_stopTime.now();
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

    Os::RawTime::Status IntervalTimer::getTimeInterval(Fw::TimeInterval& interval) const {
        return this->m_stopTime.getTimeInterval(this->m_startTime, interval);
    }
}
