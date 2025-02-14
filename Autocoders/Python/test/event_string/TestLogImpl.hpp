/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND1IMPL_HPP_
#define TESTCOMMAND1IMPL_HPP_

#include <Autocoders/Python/test/event_string/TestComponentAc.hpp>

class TestLogImpl: public Somewhere::TestLogComponentBase {
    public:
        TestLogImpl(const char* compName);
        virtual ~TestLogImpl();
        void init();
        void sendEvent(I32 arg1, Fw::LogStringArg& arg2, U8 arg3);
    protected:
        void aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
