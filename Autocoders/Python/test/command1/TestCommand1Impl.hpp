/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND1IMPL_HPP_
#define TESTCOMMAND1IMPL_HPP_

#include <Autocoders/Python/test/command1/Test1ComponentAc.hpp>

class TestCommand1Impl: public Cmd::Test1ComponentBase {
    public:
        TestCommand1Impl(const char* compName);
        virtual ~TestCommand1Impl();
        void init();
    protected:
        void aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6);
        void TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2, U8 arg3);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
