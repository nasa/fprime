/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/tlm2/TestTelemRecvImpl.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Autocoders/Python/test/tlm2/QuaternionSerializableAc.hpp>
#include <stdio.h>

#if FW_OBJECT_NAMES == 1
TestTelemRecvImpl::TestTelemRecvImpl(const char* name) : Tlm::TelemTesterComponentBase(name)
#else
TestTelemRecvImpl::TestTelemRecvImpl() :
    Tlm::TelemTesterComponentBase()
#endif
{
}

TestTelemRecvImpl::~TestTelemRecvImpl() {
}

void TestTelemRecvImpl::tlmRecvPort_handler(NATIVE_INT_TYPE portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val) {
    Ref::Gnc::Quaternion tlmVal;
    val.deserialize(tlmVal);
    Fw::EightyCharString str;
    tlmVal.toString(str);
    printf("ID: %d TLM value is %s. Time is %d:%d base: %d\n",id,str.toChar(),timeTag.getSeconds(),timeTag.getUSeconds(),timeTag.getTimeBase());
}

void TestTelemRecvImpl::init(void) {
    Tlm::TelemTesterComponentBase::init();
}


