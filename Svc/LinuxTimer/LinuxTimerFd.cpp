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

void LinuxTimer::startTimer(const Fw::TimeInterval& interval) {
    int fd;
    struct itimerspec itval;

    /* Create the timer */
    fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd == -1) {
        Fw::Logger::log("timer create error: %s\n", strerror(errno));
        return;
    }
    time_t seconds_value = static_cast<time_t>(interval.getSeconds());
    // Ensure an overflow did not occur
    FW_ASSERT(seconds_value == interval.getSeconds());
    itval.it_interval.tv_sec = static_cast<time_t>(seconds_value);
    itval.it_interval.tv_nsec = static_cast<long>(interval.getUSeconds() * 1000);
    itval.it_value.tv_sec = static_cast<time_t>(seconds_value);
    itval.it_value.tv_nsec = static_cast<long>(interval.getUSeconds() * 1000);

    const int settimeStatus = timerfd_settime(fd, 0, &itval, nullptr);
    if (settimeStatus == -1) {
        Fw::Logger::log("timer settime error: %s\n", strerror(errno));
    }

    while (true) {
        unsigned long long missed;
        int ret = static_cast<int>(read(fd, &missed, sizeof(missed)));
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

            (void)timerfd_settime(fd, 0, &itval, nullptr);  // best-effort disarm on shutdown
            (void)::close(fd);
            return;
        }
        Os::RawTime::Status rawTimeStatus = this->m_rawTime.now();
        if ((rawTimeStatus != Os::RawTime::Status::OP_OK) && !this->m_rawTimeErrorLogged) {
            // Latch the report so a persistent failure does not flood the console at the timer rate
            this->m_rawTimeErrorLogged = true;
            Fw::Logger::log("timer raw time error: %d\n", static_cast<I32>(rawTimeStatus));
        }
        this->CycleOut_out(0, this->m_rawTime);
    }
}

}  // end namespace Svc
