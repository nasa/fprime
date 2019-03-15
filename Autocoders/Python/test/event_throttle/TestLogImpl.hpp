/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND1IMPL_HPP_
#define TESTCOMMAND1IMPL_HPP_

#include <Autocoders/Python/test/event_throttle/TestComponentAc.hpp>

class TestLogImpl: public Somewhere::TestLogComponentBase {
    public:
#if FW_OBJECT_NAMES == 1
        TestLogImpl(const char* compName);
#else
        TestLogImpl();
#endif
        virtual ~TestLogImpl();
        void init(NATIVE_INT_TYPE instance);
        void sendEvent(I32 arg1, F32 arg2, U8 arg3);
        void resetEvent(void);
    protected:
        void aport_handler(NATIVE_INT_TYPE portNum, I32 arg4, F32 arg5, U8 arg6);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
