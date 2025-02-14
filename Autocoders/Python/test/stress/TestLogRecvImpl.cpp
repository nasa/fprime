/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/stress/TestLogRecvImpl.hpp>
#include <cstdio>

TestLogRecvImpl::TestLogRecvImpl(const char* name) : LogTextImpl(name)
{
}

TestLogRecvImpl::~TestLogRecvImpl() {
}

void TestLogRecvImpl::logRecvPort_handler(FwIndexType portNum, FwEventIdType id, Fw::Time &timeTag, const Fw::LogSeverity& severity, Fw::LogBuffer &args) {
    printf("Received log %d, Time (%d,%d:%d) severity %d\n",id,timeTag.getTimeBase(),timeTag.getSeconds(),timeTag.getUSeconds(),severity.e);
    I32 arg1;
    F32 arg2;
    U8 arg3;

    // deserialize them in reverse order
    args.deserialize(arg3);
    args.deserialize(arg2);
    args.deserialize(arg1);
    printf("Args: %d %f %c\n",arg1,arg2,arg3);


}

void TestLogRecvImpl::init() {
    LogTextImpl::init();
}


