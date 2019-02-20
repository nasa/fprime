/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/log_tester/TestTextLogImpl.hpp>
#include <stdio.h>

#if FW_OBJECT_NAMES == 1
LogTextImpl::LogTextImpl(const char* name) : Log::LogTesterComponentBase(name)
#else
LogTextImpl::LogTextImpl() :
        Log::LogTesterComponentBase()
#endif
{
}

LogTextImpl::~LogTextImpl() {
}

void LogTextImpl::init(void) {
    Log::LogTesterComponentBase::init();
}

void LogTextImpl::textLogRecvPort_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::TextLogSeverity severity, Fw::TextLogString &text) {
    printf("Log id %d, time (%d,%d:%d), severity %d, text \"%s\"",id,timeTag.getTimeBase(),timeTag.getSeconds(),timeTag.getUSeconds(),severity,text.toChar());
}


