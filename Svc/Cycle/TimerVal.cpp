/*
 * TimerVal.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: timothycanham
 */

#include <Svc/Cycle/TimerVal.hpp>
#include <cstring>

namespace Svc {

    TimerVal::TimerVal() : Fw::Serializable() {
        this->m_timerVal.upper = 0;
        this->m_timerVal.lower = 0;
    }

    TimerVal::TimerVal(U32 upper, U32 lower)  {
        this->m_timerVal.upper = upper;
        this->m_timerVal.lower = lower;
    }

    TimerVal::TimerVal(const TimerVal& other) : Fw::Serializable() {
        this->m_timerVal.upper = other.m_timerVal.upper;
        this->m_timerVal.lower = other.m_timerVal.lower;
    }

    TimerVal& TimerVal::operator=(const TimerVal& other) {
        this->m_timerVal.upper = other.m_timerVal.upper;
        this->m_timerVal.lower = other.m_timerVal.lower;
        return *this;
    }

    Os::IntervalTimer::RawTime TimerVal::getTimerVal() const {
    	return this->m_timerVal;
    }

    void TimerVal::take() {
        Os::IntervalTimer::getRawTime(this->m_timerVal);
    }

    U32 TimerVal::diffUSec(const TimerVal& time) {
        return Os::IntervalTimer::getDiffUsec(this->m_timerVal,time.m_timerVal);
    }

    Fw::SerializeStatus TimerVal::serialize(Fw::SerializeBufferBase& buffer) const {
        Fw::SerializeStatus stat = buffer.serialize(this->m_timerVal.upper);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
        return buffer.serialize(this->m_timerVal.lower);
    }

    Fw::SerializeStatus TimerVal::deserialize(Fw::SerializeBufferBase& buffer) {
        Fw::SerializeStatus stat = buffer.deserialize(this->m_timerVal.upper);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
        return buffer.deserialize(this->m_timerVal.lower);
    }



} /* namespace Svc */
