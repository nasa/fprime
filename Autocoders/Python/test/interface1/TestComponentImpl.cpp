/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/interface1/TestComponentImpl.hpp>
#include <cstdio>

TestComponentImpl::TestComponentImpl(const char* name) : TestComponentBase(name)
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

void TestComponentImpl::test2_internalInterfaceHandler(I32 arg1, SomeEnum arg2, const Fw::InternalInterfaceString& arg3, const ANameSpace::UserSerializer& arg4) {

}


TestComponentImpl impl("impl");
