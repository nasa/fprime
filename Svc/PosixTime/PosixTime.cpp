/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Svc/PosixTime/PosixTime.hpp>
#include <Fw/Time/Time.hpp>
#include <ctime>

namespace Svc {

    PosixTime::PosixTime(const char* name) : PosixTimeComponentBase(name)
    {
    }

    PosixTime::~PosixTime() {
    }

    void PosixTime::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        timespec stime;
        (void)clock_gettime(CLOCK_REALTIME,&stime);
        time.set(TB_WORKSTATION_TIME,0, static_cast<U32>(stime.tv_sec), static_cast<U32>(stime.tv_nsec/1000));
    }
}
