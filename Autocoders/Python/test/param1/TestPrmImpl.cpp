/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/param1/TestPrmImpl.hpp>
#include <stdio.h>

#if FW_OBJECT_NAMES == 1
TestPrmImpl::TestPrmImpl(const char* name) : Prm::TestPrmComponentBase(name)
#else
TestPrmImpl::TestPrmImpl() : Prm::TestPrmComponentBase()
#endif
{
}

TestPrmImpl::~TestPrmImpl() {
}

void TestPrmImpl::init(void) {
    Prm::TestPrmComponentBase::init();
}

void TestPrmImpl::aport_handler(NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6) {

}

void TestPrmImpl::printParam(void) {
    Fw::ParamValid valid = Fw::PARAM_INVALID;
    const U32& prmRef = this->paramGet_someparam(valid);

    printf("Parameter is: %d %s\n",prmRef,valid==Fw::PARAM_VALID?"VALID":"INVALID");
}
