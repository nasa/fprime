/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Fw/Time/Time.hpp>
#include <Svc/PosixTime/PosixTime.hpp>
#include <ctime>

namespace Svc {

PosixTime::PosixTime(const char* name) : PosixTimeComponentBase(name) {}

PosixTime::~PosixTime() {}

void PosixTime::timeGetPort_handler(FwIndexType portNum, /*!< The port number*/
                                    Fw::Time& time       /*!< The U32 cmd argument*/
) {
    timespec stime = {};
    if (clock_gettime(CLOCK_REALTIME, &stime) != 0) {
        // report zero time rather than uninitialized garbage
        stime.tv_sec = 0;
        stime.tv_nsec = 0;
    }
    time.set(TimeBase::TB_WORKSTATION_TIME, 0, static_cast<U32>(stime.tv_sec), static_cast<U32>(stime.tv_nsec / 1000));
}
}  // namespace Svc
