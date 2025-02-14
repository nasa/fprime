/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/param_enum/TestPrmImpl.hpp>
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
    SomeEnum val = this->paramGet_enumparam(valid);

    printf("Parameter is: %d %s\n",val,valid==Fw::ParamValid::VALID?"VALID":"INVALID");
}
