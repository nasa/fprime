// ======================================================================
// \title  LinuxTimerImpl.cpp
// \author tim
// \brief  cpp file for LinuxTimer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/FPrimeBasicTypes.hpp>
#include <Os/Task.hpp>
#include <Svc/LinuxTimer/LinuxTimer.hpp>

namespace Svc {

void LinuxTimer::startTimer(FwSizeType interval) {
    FW_ASSERT(std::numeric_limits<U32>::max() / 1000 >= interval);  // Overflow
    while (true) {
        Os::Task::delay(
            Fw::TimeInterval(static_cast<U32>(interval / 1000), static_cast<U32>((interval % 1000) * 1000)));
        this->m_mutex.lock();
        bool quit = this->m_quit;
        this->m_mutex.unLock();
        if (quit) {
            return;
        }
        this->m_rawTime.now();
        this->CycleOut_out(0, this->m_rawTime);
    }
}

}  // end namespace Svc
