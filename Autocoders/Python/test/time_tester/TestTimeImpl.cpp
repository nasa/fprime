/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/time_tester/TestTimeImpl.hpp>
#include <cstdio>

TestTimeImpl::TestTimeImpl(const char* name) : Time::TimeTesterComponentBase(name)
{
}

TestTimeImpl::~TestTimeImpl() {
}

void TestTimeImpl::timeGetPort_handler(
        FwIndexType portNum, /*!< The port number*/
        Fw::Time &time /*!< The U32 cmd argument*/
    ) {
    time = this->m_time;
}

void TestTimeImpl::init() {
    Time::TimeTesterComponentBase::init();
}

void TestTimeImpl::setTime(Fw::Time time) {
    printf("Setting time to (%d,%d,%d)\n",time.getSeconds(),time.getUSeconds(),time.getTimeBase());
    this->m_time = time;
}


