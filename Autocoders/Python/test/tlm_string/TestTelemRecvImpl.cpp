/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/tlm_string/TestTelemRecvImpl.hpp>
#include <stdio.h>
#include <Fw/Tlm/TlmString.hpp>

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
    Fw::TlmString tlmVal;

    Fw::SerializeStatus stat = val.deserialize(tlmVal);;
    if (stat != Fw::FW_SERIALIZE_OK) {
        printf("Deser failed: %d\n",stat);
    }
    printf("ID: %d TLM value is \"%s\". Time is %d:%d base: %d\n",id,tlmVal.toChar(),timeTag.getSeconds(),timeTag.getUSeconds(),timeTag.getTimeBase());
}

void TestTelemRecvImpl::init(void) {
    Tlm::TelemTesterComponentBase::init();
}


