/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/tlm_onchange/TestTelemRecvImpl.hpp>
#include <cstdio>

TestTelemRecvImpl::TestTelemRecvImpl(const char* name) : Tlm::TelemTesterComponentBase(name)
{
}

TestTelemRecvImpl::~TestTelemRecvImpl() {
}

void TestTelemRecvImpl::tlmRecvPort_handler(FwIndexType portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val) {
    U32 tlmVal;
    val.deserialize(tlmVal);
    printf("ID: %d TLM value is %d. Time is %d:%d base: %d\n",id,tlmVal,timeTag.getSeconds(),timeTag.getUSeconds(),timeTag.getTimeBase());
}

void TestTelemRecvImpl::init() {
    Tlm::TelemTesterComponentBase::init();
}


