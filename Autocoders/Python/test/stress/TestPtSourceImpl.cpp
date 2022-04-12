/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/stress/TestPtSourceImpl.hpp>
#include <cstdio>

TestPtSourceImpl::TestPtSourceImpl(const char* name) : StressTest::TestPortComponentBase(name)
{
}

TestPtSourceImpl::~TestPtSourceImpl() {
}

void TestPtSourceImpl::init() {
    StressTest::TestPortComponentBase::init();
}

void TestPtSourceImpl::aport_Test(I32 arg4, F32 arg5, U8 arg6) {
    if (this->isConnected_aport_OutputPort(0)) {
        this->aport_out(0,arg4,arg5,arg6);
    }
}

void TestPtSourceImpl::aport2_Test2(I32 arg4, F32 arg5, const Ref::Gnc::Quaternion& arg6) {
    if (this->isConnected_aport2_OutputPort(0)) {
        this->aport2_out(0,arg4,arg5,arg6);
    }

}

