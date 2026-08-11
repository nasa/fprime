/*
 * PosixTime.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Svc/PosixTime/PosixTime.hpp>
#include <Fw/Time/Time.hpp>
#include <ctime>

namespace Svc {

    PosixTime::PosixTime(const char* name) : PosixTimeComponentBase(name), m_timeContext(0)
    {
    }

    PosixTime::~PosixTime() {
    }

    void PosixTime::setTimeContext(FwTimeContextStoreType timeContext) {
        this->m_timeContext = timeContext;
    }

    void PosixTime::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        timespec stime;
        (void)clock_gettime(CLOCK_REALTIME,&stime);
        time.set(TB_WORKSTATION_TIME, this->m_timeContext, static_cast<U32>(stime.tv_sec), static_cast<U32>(stime.tv_nsec/1000));
    }
}
