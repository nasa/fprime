/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/tlm_string/TestTelemRecvImpl.hpp>
#include <cstdio>
#include <Fw/Tlm/TlmString.hpp>

TestTelemRecvImpl::TestTelemRecvImpl(const char* name) : Tlm::TelemTesterComponentBase(name)
{
}

TestTelemRecvImpl::~TestTelemRecvImpl() {
}

void TestTelemRecvImpl::tlmRecvPort_handler(FwIndexType portNum, FwChanIdType id, Fw::Time &timeTag, Fw::TlmBuffer &val) {
    Fw::TlmString tlmVal;

    Fw::SerializeStatus stat = val.deserialize(tlmVal);;
    if (stat != Fw::FW_SERIALIZE_OK) {
        printf("Deser failed: %d\n",stat);
    }
    printf("ID: %d TLM value is \"%s\". Time is %d:%d base: %d\n",id,tlmVal.toChar(),timeTag.getSeconds(),timeTag.getUSeconds(),timeTag.getTimeBase());
}

void TestTelemRecvImpl::init() {
    Tlm::TelemTesterComponentBase::init();
}


