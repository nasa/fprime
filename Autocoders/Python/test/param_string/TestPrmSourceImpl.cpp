/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/param_string/TestPrmSourceImpl.hpp>
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

void TestParamSourceImpl::setPrm(Fw::ParamString& val) {
    printf("Setting parameter to %s\n",val.toChar());
    this->m_prm.resetSer();
    Fw::SerializeStatus stat = this->m_prm.serialize(val);

    if (stat != Fw::FW_SERIALIZE_OK) {
        printf("Error serializing: %d\n",stat);
    }
}


