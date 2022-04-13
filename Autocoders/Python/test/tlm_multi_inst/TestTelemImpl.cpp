/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/tlm_multi_inst/TestTelemImpl.hpp>
#include <cstdio>

TestTlmImpl::TestTlmImpl(const char* name) : Tlm::TestTlmComponentBase(name)
{
}

TestTlmImpl::~TestTlmImpl() {
}

void TestTlmImpl::init() {
    Tlm::TestTlmComponentBase::init();
}

void TestTlmImpl::genTlm(U32 val) {
    printf("Writing value %d to telemetry.\n",val);
    this->tlmWrite_somechan(val);
}

void TestTlmImpl::aport_handler(NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6) {

}
