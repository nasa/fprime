/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/event_string/TestLogImpl.hpp>
#include <cstdio>

TestLogImpl::TestLogImpl(const char* name) : Somewhere::TestLogComponentBase(name)
{
}

TestLogImpl::~TestLogImpl() {
}

void TestLogImpl::init() {
    Somewhere::TestLogComponentBase::init();
}


void TestLogImpl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {

}

void TestLogImpl::sendEvent(I32 arg1, Fw::LogStringArg& arg2, U8 arg3) {
    printf("Sending event args %d, %s, %d\n",arg1, arg2.toChar(), arg3);
    this->log_ACTIVITY_LO_SomeEvent(arg1,arg2,arg3);
}
