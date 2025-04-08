/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/tlm2/TestTelemRecvImpl.hpp>
#include <Fw/Types/String.hpp>
#include <Autocoders/Python/test/tlm2/QuaternionSerializableAc.hpp>
#include <cstdio>

TestTelemRecvImpl::TestTelemRecvImpl(const char* name) : Tlm::TelemTesterComponentBase(name)
{
}

TestTelemRecvImpl::~TestTelemRecvImpl() {
}

void TestTelemRecvImpl::tlmRecvPort_handler(FwIndexType portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val) {
    Ref::Gnc::Quaternion tlmVal;
    val.deserialize(tlmVal);
    Fw::String str;
    tlmVal.toString(str);
    printf("ID: %d TLM value is %s. Time is %d:%d base: %d\n",id,str.toChar(),timeTag.getSeconds(),timeTag.getUSeconds(),timeTag.getTimeBase());
}

void TestTelemRecvImpl::init() {
    Tlm::TelemTesterComponentBase::init();
}


