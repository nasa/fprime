/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/param1/TestPrmImpl.hpp>
#include <cstdio>

TestPrmImpl::TestPrmImpl(const char* name) : Prm::TestPrmComponentBase(name)
{
}

TestPrmImpl::~TestPrmImpl() {
}

void TestPrmImpl::init() {
    Prm::TestPrmComponentBase::init();
}

void TestPrmImpl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {

}

void TestPrmImpl::printParam() {
    Fw::ParamValid valid = Fw::ParamValid::INVALID;
    const U32& prmRef = this->paramGet_someparam(valid);

    printf("Parameter is: %d %s\n",prmRef,valid==Fw::ParamValid::VALID?"VALID":"INVALID");
}
