/*
 * TestCommand1Impl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTCOMMAND1IMPL_HPP_
#define TESTCOMMAND1IMPL_HPP_

#include <Autocoders/Python/test/command_multi_inst/Test1ComponentAc.hpp>

class TestCommand1Impl: public Cmd::Test1ComponentBase {
    public:
        TestCommand1Impl(const char* compName);
        virtual ~TestCommand1Impl();
        void init(NATIVE_INT_TYPE queueDepth, //!< The queue depth
                NATIVE_INT_TYPE instance = 0);
    protected:
        void aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6);
        void TEST_CMD_1_cmdHandler(
                FwOpcodeType opCode, /*!< The opcode*/
                U32 cmdSeq, /*!< The command sequence number*/
                I32 arg1, /*!< The I32 command argument*/
                F32 arg2, /*!< The F32 command argument*/
                U8 arg3, /*!< The U8 command argument*/
                SomeEnum arg4 /*!< The ENUM argument*/
            );
        void TEST_CMD_2_cmdHandler(
                FwOpcodeType opCode, //!< The opcode
                U32 cmdSeq, //!< The command sequence number
                U32 arg1, //!< The U32 command argument
                F64 arg2, //!< The F64 command argument
                I8 arg3 //!< The I8 command argument
            );
};

#endif /* TESTCOMMAND1IMPL_HPP_ */
