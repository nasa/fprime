/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Svc/LinuxTime/LinuxTimeImpl.hpp>
#include <Fw/Time/Time.hpp>
#include <time.h>

namespace Svc {

    #if FW_OBJECT_NAMES == 1
    LinuxTimeImpl::LinuxTimeImpl(const char* name) : TimeComponentBase(name)
    #else
    LinuxTimeImpl::LinuxTimeImpl()
    #endif
    {
    }

    LinuxTimeImpl::~LinuxTimeImpl() {
    }

    void LinuxTimeImpl::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        timespec stime;
        (void)clock_gettime(CLOCK_REALTIME,&stime);
        time.set(TB_WORKSTATION_TIME,0, stime.tv_sec, stime.tv_nsec/1000);
    }

    void LinuxTimeImpl::init(NATIVE_INT_TYPE instance) {
        TimeComponentBase::init(instance);
    }

}
