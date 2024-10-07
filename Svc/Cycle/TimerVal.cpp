/*
 * TimerVal.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: timothycanham
 */

#include <Svc/Cycle/TimerVal.hpp>
#include <cstring>

namespace Svc {


    // TimerVal::TimerVal(U32 upper, U32 lower)  {
    //     this->m_timerVal.upper = upper;
    //     this->m_timerVal.lower = lower;
    // } // TODO: figure something out for this test constructor

    TimerVal::TimerVal() : Fw::Serializable(), m_rawTime() {}

    TimerVal::TimerVal(const TimerVal& other) : Fw::Serializable() {
        this->m_rawTime = other.m_rawTime;
    }

    TimerVal& TimerVal::operator=(const TimerVal& other) {
        this->m_rawTime = other.m_rawTime;
        return *this;
    }

    Os::RawTime TimerVal::getTimerVal() const {
        return this->m_rawTime;
    }

    void TimerVal::take() {
        this->m_rawTime.getRawTime();
    }

    U32 TimerVal::diffUSec(const TimerVal& time) {
        U32 result = 0;
        this->m_rawTime.getDiffUsec(time.m_rawTime, result);
        return result;
    }

    Fw::SerializeStatus TimerVal::serialize(Fw::SerializeBufferBase& buffer) const {
        Fw::SerializeStatus stat = buffer.serialize(this->m_rawTime);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
        return buffer.serialize(this->m_rawTime);
    }

    Fw::SerializeStatus TimerVal::deserialize(Fw::SerializeBufferBase& buffer) {
        Fw::SerializeStatus stat = buffer.deserialize(this->m_rawTime);
        if (stat != Fw::FW_SERIALIZE_OK) {
            return stat;
        }
        return buffer.deserialize(this->m_rawTime);
    }



} /* namespace Svc */
