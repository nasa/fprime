/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/stress/TestPtSourceImpl.hpp>
#include <stdio.h>

#if FW_OBJECT_NAMES == 1
TestPtSourceImpl::TestPtSourceImpl(const char* name) : StressTest::TestPortComponentBase(name)
#else
TestPtSourceImpl::TestPtSourceImpl() :
StressTest::TestPortComponentBase()
#endif
{
}

TestPtSourceImpl::~TestPtSourceImpl() {
}

void TestPtSourceImpl::init(void) {
    StressTest::TestPortComponentBase::init();
}

void TestPtSourceImpl::aport_Test(I32 arg4, F32 arg5, U8 arg6) {
    if (this->isConnected_aport_OutputPort(0)) {
        this->aport_out(0,arg4,arg5,arg6);
    }
}

void TestPtSourceImpl::aport2_Test2(I32 arg4, F32 arg5, Ref::Gnc::Quaternion arg6) {
    if (this->isConnected_aport2_OutputPort(0)) {
        this->aport2_out(0,arg4,arg5,arg6);
    }

}

