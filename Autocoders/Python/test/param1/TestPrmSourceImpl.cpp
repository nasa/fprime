/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/param1/TestPrmSourceImpl.hpp>
#include <cstdio>

TestParamSourceImpl::TestParamSourceImpl(const char* name) : Prm::ParamTesterComponentBase(name)
{
}

TestParamSourceImpl::~TestParamSourceImpl() {
}

Fw::ParamValid TestParamSourceImpl::paramGetPort_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer &val) {

    val = this->m_prm;
    return Fw::ParamValid::VALID;
}

void TestParamSourceImpl::paramSetPort_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer &val) {

}

void TestParamSourceImpl::init() {
    Prm::ParamTesterComponentBase::init();
}

void TestParamSourceImpl::setPrm(U32 val) {
    printf("Setting parameter to %d\n",val);
    this->m_prm.resetSer();
    this->m_prm.serialize(val);
}


