/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/tlm_enum/TestTelemImpl.hpp>
#include <cstdio>

TestTlmImpl::TestTlmImpl(const char* name) : Tlm::TestTlmComponentBase(name)
{
}

TestTlmImpl::~TestTlmImpl() {
}

void TestTlmImpl::init() {
    Tlm::TestTlmComponentBase::init();
}

void TestTlmImpl::genTlm(I32 val) {
    printf("Writing value %d to telemetry.\n",val);
    SomeEnum argVal = static_cast<SomeEnum>(val);
    this->tlmWrite_somechan(argVal);
}

void TestTlmImpl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {

}
