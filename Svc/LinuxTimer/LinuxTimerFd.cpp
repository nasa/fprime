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

#include <sys/timerfd.h>
#include <unistd.h>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Svc/LinuxTimer/LinuxTimer.hpp>
#include <cerrno>
#include <cstring>

namespace Svc {

void LinuxTimer::startTimer(FwSizeType interval) {
    int fd;
    struct itimerspec itval;

    /* Create the timer */
    fd = timerfd_create(CLOCK_MONOTONIC, 0);
    const FwSizeType interval_secs = interval / 1000;
    FW_ASSERT(static_cast<FwSizeType>(std::numeric_limits<I32>::max()) >= interval_secs,
              static_cast<FwAssertArgType>(interval));
    itval.it_interval.tv_sec = static_cast<I32>(interval_secs);
    itval.it_interval.tv_nsec = static_cast<I32>((interval * 1000000) % 1000000000);
    itval.it_value.tv_sec = static_cast<I32>(interval_secs);
    itval.it_value.tv_nsec = static_cast<I32>((interval * 1000000) % 1000000000);

    timerfd_settime(fd, 0, &itval, nullptr);

    while (true) {
        unsigned long long missed;
        int ret = static_cast<int>(read(fd, &missed, sizeof(missed)));
        if (-1 == ret) {
            Fw::Logger::log("timer read error: %s\n", strerror(errno));
        }
        this->m_mutex.lock();
        bool quit = this->m_quit;
        this->m_mutex.unLock();
        if (quit) {
            itval.it_interval.tv_sec = 0;
            itval.it_interval.tv_nsec = 0;
            itval.it_value.tv_sec = 0;
            itval.it_value.tv_nsec = 0;

            timerfd_settime(fd, 0, &itval, nullptr);
            return;
        }
        this->m_rawTime.now();
        this->CycleOut_out(0, this->m_rawTime);
    }
}

}  // end namespace Svc
