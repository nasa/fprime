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
      if (fd == -1) {
          Fw::Logger::log("timer create error: %s\n", strerror(errno));
          return;
      }

      itval.it_interval.tv_sec = interval/1000;
      itval.it_interval.tv_nsec = (interval*1000000)%1000000000;
      itval.it_value.tv_sec = interval/1000;
      itval.it_value.tv_nsec = (interval*1000000)%1000000000;

      timerfd_settime (fd, 0, &itval, nullptr);

      while (true) {
          unsigned long long missed;
          int ret = static_cast<int>(read (fd, &missed, sizeof (missed)));
          if ((-1 == ret) && (errno != EINTR)) {
              // A non-interrupt read error will not clear itself; stop rather than spin on it
              Fw::Logger::log("timer read error: %s\n", strerror(errno));
              (void)::close(fd);
              return;
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
              (void)::close(fd);
              return;
          }
          this->m_rawTime.now();
          this->CycleOut_out(0,this->m_rawTime);
      }
  }

} // end namespace Svc
