/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/tlm2/TestTelemImpl.hpp>
#include <stdio.h>

#if FW_OBJECT_NAMES == 1
TestTlmImpl::TestTlmImpl(const char* name) : Tlm::TestTlmComponentBase(name)
#else
TestTlmImpl::TestTlmImpl() : Tlm::TestTlmComponentBase()
#endif
{
}

TestTlmImpl::~TestTlmImpl() {
}

void TestTlmImpl::init(void) {
    Tlm::TestTlmComponentBase::init();
}

void TestTlmImpl::genTlm(Ref::Gnc::Quaternion val) {
    this->tlmWrite_AQuat(val);
}

void TestTlmImpl::aport_handler(NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6) {

}
