/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND2IMPL_HPP_
#define TESTCOMMAND2IMPL_HPP_

#include <Autocoders/Python/test/stress/TestComponentAc.hpp>

class TestCommand1Impl: public StressTest::TestCommandComponentBase {
    public:
        TestCommand1Impl(const char* compName);
        void init(NATIVE_INT_TYPE queueDepth);
        virtual ~TestCommand1Impl();
        void printParam();
        void genTlm(Ref::Gnc::Quaternion val);
        void sendEvent(I32 arg1, F32 arg2, U8 arg3);
    private:
        void aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6);
        void aport2_handler(FwIndexType portNum, I32 arg4, F32 arg5, const Ref::Gnc::Quaternion& arg6);
        void TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, StressTest::TestCommandComponentBase::SomeEnum arg2);
        void TEST_CMD_2_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2);
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
