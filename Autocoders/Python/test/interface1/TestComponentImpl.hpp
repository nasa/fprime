/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND1IMPL_HPP_
#define TESTCOMMAND1IMPL_HPP_

#include <Autocoders/Python/test/interface1/TestComponentAc.hpp>

class TestComponentImpl: public Cmd::TestComponentBase {
    public:
        TestComponentImpl(const char* compName);
        virtual ~TestComponentImpl();
        void init(NATIVE_INT_TYPE queueDepth);
    protected:
        void test_internalInterfaceHandler(I32 arg1, F32 arg2, U8 arg3);
        void test2_internalInterfaceHandler(I32 arg1, SomeEnum arg2, const Fw::InternalInterfaceString& arg3, const ANameSpace::UserSerializer& arg4);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
