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

#include <Fw/Logger/Logger.hpp>
#include <Svc/LinuxTimer/LinuxTimerComponentImpl.hpp>
#include <FpConfig.hpp>
#include <sys/timerfd.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace Svc {

  void LinuxTimerComponentImpl::startTimer(NATIVE_INT_TYPE interval) {
      int fd;
      struct itimerspec itval;

      /* Create the timer */
      fd = timerfd_create (CLOCK_MONOTONIC, 0);

      itval.it_interval.tv_sec = interval/1000;
      itval.it_interval.tv_nsec = (interval*1000000)%1000000000;
      itval.it_value.tv_sec = interval/1000;
      itval.it_value.tv_nsec = (interval*1000000)%1000000000;

      timerfd_settime (fd, 0, &itval, nullptr);

      while (true) {
          unsigned long long missed;
          int ret = read (fd, &missed, sizeof (missed));
          if (-1 == ret) {
              Fw::Logger::logMsg("timer read error: %s\n", reinterpret_cast<POINTER_CAST>(strerror(errno)));
          }
          this->m_mutex.lock();
          bool quit = this->m_quit;
          this->m_mutex.unLock();
          if (quit) {
              itval.it_interval.tv_sec = 0;
              itval.it_interval.tv_nsec = 0;
              itval.it_value.tv_sec = 0;
              itval.it_value.tv_nsec = 0;

              timerfd_settime (fd, 0, &itval, nullptr);
              return;
          }
          this->m_timer.take();
          this->CycleOut_out(0,this->m_timer);
      }
  }

} // end namespace Svc
