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
#include <Svc/LinuxTimer/LinuxTimer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

LinuxTimer ::LinuxTimer(const char* const compName) : LinuxTimerComponentBase(compName), m_quit(false) {}

LinuxTimer ::~LinuxTimer() {}

void LinuxTimer::quit() {
    this->m_mutex.lock();
    this->m_quit = true;
    this->m_mutex.unLock();
}

}  // end namespace Svc
