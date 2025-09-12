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

void LinuxTimer::startTimer(const Fw::TimeInterval& interval) {
    while (true) {
        Os::Task::delay(interval);
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
