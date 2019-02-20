/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/time_tester/TestTimeImpl.hpp>
#include <stdio.h>

#if FW_OBJECT_NAMES == 1
TestTimeImpl::TestTimeImpl(const char* name) : Time::TimeTesterComponentBase(name)
#else
TestTimeImpl::TestTimeImpl() :
Time::TimeTesterComponentBase()
#endif
{
}

TestTimeImpl::~TestTimeImpl() {
}

void TestTimeImpl::timeGetPort_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Time &time /*!< The U32 cmd argument*/
    ) {
    time = this->m_time;
}

void TestTimeImpl::init(void) {
    Time::TimeTesterComponentBase::init();
}

void TestTimeImpl::setTime(Fw::Time time) {
    printf("Setting time to (%d,%d,%d)\n",time.getSeconds(),time.getUSeconds(),time.getTimeBase());
    this->m_time = time;
}


