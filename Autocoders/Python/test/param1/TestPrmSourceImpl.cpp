/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/param1/TestPrmSourceImpl.hpp>
#include <stdio.h>

#if FW_OBJECT_NAMES == 1
TestParamSourceImpl::TestParamSourceImpl(const char* name) : Prm::ParamTesterComponentBase(name)
#else
TestParamSourceImpl::TestParamSourceImpl() :
        Prm::ParamTesterComponentBase()
#endif
{
}

TestParamSourceImpl::~TestParamSourceImpl() {
}

Fw::ParamValid TestParamSourceImpl::paramGetPort_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val) {

    val = this->m_prm;
    return Fw::PARAM_VALID;
}

void TestParamSourceImpl::paramSetPort_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val) {

}

void TestParamSourceImpl::init(void) {
    Prm::ParamTesterComponentBase::init();
}

void TestParamSourceImpl::setPrm(U32 val) {
    printf("Setting parameter to %d\n",val);
    this->m_prm.resetSer();
    this->m_prm.serialize(val);
}


