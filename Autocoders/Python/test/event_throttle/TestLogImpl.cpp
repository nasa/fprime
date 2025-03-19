/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/event_throttle/TestLogImpl.hpp>
#include <cstdio>

TestLogImpl::TestLogImpl(const char* name) : Somewhere::TestLogComponentBase(name)
{
}

TestLogImpl::~TestLogImpl() {
}

void TestLogImpl::init(NATIVE_INT_TYPE instance) {
    Somewhere::TestLogComponentBase::init(instance);
}


void TestLogImpl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {

}

void TestLogImpl::sendEvent(I32 arg1, F32 arg2, U8 arg3) {
    printf("Sending event args %d, %f, %d\n",arg1, arg2, arg3);
    this->log_DIAGNOSTIC_SomeEvent(arg1,arg2,arg3);
}

void TestLogImpl::resetEvent() {
    this->log_DIAGNOSTIC_SomeEvent_ThrottleClear();
}

