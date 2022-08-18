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


#include <Svc/LinuxTimer/LinuxTimerComponentImpl.hpp>
#include <FpConfig.hpp>
#include <Os/Task.hpp>

namespace Svc {

  void LinuxTimerComponentImpl::startTimer(NATIVE_INT_TYPE interval) {
      while (true) {
          Os::Task::delay(interval);
          this->m_mutex.lock();
          bool quit = this->m_quit;
          this->m_mutex.unLock();
          if (quit) {
              return;
          }
          this->m_timer.take();
          this->CycleOut_out(0,this->m_timer);
      }
  }

} // end namespace Svc
