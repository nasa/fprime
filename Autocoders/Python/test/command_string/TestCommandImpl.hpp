/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND2IMPL_HPP_
#define TESTCOMMAND2IMPL_HPP_

#include <Autocoders/Python/test/command_string/TestComponentAc.hpp>

class TestCommand1Impl: public AcTest::TestCommandComponentBase {
    public:
        TestCommand1Impl(const char* compName);
        void init(NATIVE_INT_TYPE queueDepth);
        virtual ~TestCommand1Impl();
        void aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6);
        void TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, const Fw::CmdStringArg& arg2, SomeEnum arg3);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
