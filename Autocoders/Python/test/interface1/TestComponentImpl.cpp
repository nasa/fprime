/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/interface1/TestComponentImpl.hpp>
#include <cstdio>

#if FW_OBJECT_NAMES == 1
TestComponentImpl::TestComponentImpl(const char* name) : TestComponentBase(name)
#else
TestComponentImpl::TestComponentImpl() : TestComponentBase()
#endif
{
    // TODO Auto-generated constructor stub

}

TestComponentImpl::~TestComponentImpl() {
    // TODO Auto-generated destructor stub
}

void TestComponentImpl::init(NATIVE_INT_TYPE queueDepth) {
    TestComponentBase::init(queueDepth);
}

void TestComponentImpl::test_internalInterfaceHandler(I32 arg1, F32 arg2, U8 arg3) {

}

void TestComponentImpl::test2_internalInterfaceHandler(I32 arg1, SomeEnum arg2, const Fw::InternalInterfaceString& arg3, ANameSpace::UserSerializer& arg4) {

}


TestComponentImpl impl("impl");
