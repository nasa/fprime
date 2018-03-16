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
#include <sys/timerfd.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

namespace Svc {

  void LinuxTimerComponentImpl::startTimer(NATIVE_INT_TYPE interval) {
      int fd;
      struct itimerspec itval;

      /* Create the timer */
      fd = timerfd_create (CLOCK_MONOTONIC, 0);

      itval.it_interval.tv_sec = interval/1000;
      itval.it_interval.tv_nsec = interval*1000000;
      itval.it_value.tv_sec = interval/1000;
      itval.it_value.tv_nsec = interval*1000000;

      timerfd_settime (fd, 0, &itval, NULL);

      while (1) {
          unsigned long long missed;
          int ret = read (fd, &missed, sizeof (missed));
          if (-1 == ret) {
              printf("timer read error: %s\n",strerror(errno));
          }
          if (this->m_quit) {
              itval.it_interval.tv_sec = 0;
              itval.it_interval.tv_nsec = 0;
              itval.it_value.tv_sec = 0;
              itval.it_value.tv_nsec = 0;

              timerfd_settime (fd, 0, &itval, NULL);
              return;
          }
          this->m_timer.take();
          this->CycleOut_out(0,this->m_timer);
      }
  }

} // end namespace Svc
