// ====================================================================== 
// \title  LinuxTimerImpl.cpp
// \author tim
// \brief  cpp file for LinuxTimer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 


#include <Svc/LinuxTimer/LinuxTimerComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <Os/Task.hpp>

namespace Svc {

  void LinuxTimerComponentImpl::startTimer(NATIVE_INT_TYPE interval) {
      while (1) {
          Os::Task::delay(interval);
          if (this->m_quit) {
              return;
          }
          this->m_timer.take();
          this->CycleOut_out(0,this->m_timer);
      }
  }

} // end namespace Svc
