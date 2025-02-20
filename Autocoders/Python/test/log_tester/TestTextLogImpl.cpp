/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/log_tester/TestTextLogImpl.hpp>
#include <cstdio>

LogTextImpl::LogTextImpl(const char* name) : Log::LogTesterComponentBase(name)
{
}

LogTextImpl::~LogTextImpl() {
}

void LogTextImpl::init() {
    Log::LogTesterComponentBase::init();
}

void LogTextImpl::textLogRecvPort_handler(FwIndexType portNum, FwEventIdType id, Fw::Time &timeTag, const Fw::LogSeverity &severity, Fw::TextLogString &text) {
    printf("Log id %d, time (%d,%d:%d), severity %d, text \"%s\"",id,timeTag.getTimeBase(),timeTag.getSeconds(),timeTag.getUSeconds(),severity.e,text.toChar());
}


