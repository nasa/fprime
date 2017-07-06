/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Svc/LinuxTime/LinuxTimeImpl.hpp>
#include <Fw/Time/Time.hpp>
#include <sys/time.h>

namespace Svc {

    #if FW_OBJECT_NAMES == 1
    LinuxTimeImpl::LinuxTimeImpl(const char* name) : TimeComponentBase(name)
    #else
    LinuxTimeImpl::LinuxTimeImpl() :
            Svc::TimeComponentBase()
    #endif
    {
    }

    LinuxTimeImpl::~LinuxTimeImpl() {
    }

    void LinuxTimeImpl::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        timeval stime;
        (void)gettimeofday(&stime,0);
        time.set(TB_WORKSTATION_TIME,0,stime.tv_sec, stime.tv_usec);
    }

    void LinuxTimeImpl::init(NATIVE_INT_TYPE instance) {
        Svc::TimeComponentBase::init(instance);
    }

}
