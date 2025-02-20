/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/command_multi_inst/TestCommand1Impl.hpp>
#include <cstdio>

TestCommand1Impl::TestCommand1Impl(const char* name) : Test1ComponentBase(name)
{
    // TODO Auto-generated constructor stub

}

TestCommand1Impl::~TestCommand1Impl() {
    // TODO Auto-generated destructor stub
}

void TestCommand1Impl::init(NATIVE_INT_TYPE queueDepth, //!< The queue depth
        NATIVE_INT_TYPE instance) {
    Test1ComponentBase::init(queueDepth,instance);
}

void TestCommand1Impl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {
}

void TestCommand1Impl::TEST_CMD_1_cmdHandler(
        FwOpcodeType opCode, /*!< The opcode*/
        U32 cmdSeq, /*!< The command sequence number*/
        I32 arg1, /*!< The I32 command argument*/
        F32 arg2, /*!< The F32 command argument*/
        U8 arg3, /*!< The U8 command argument*/
        SomeEnum arg4 /*!< The ENUM argument*/
    ) {
	printf("Got command args: %d %f %d %d\n", arg1, arg2, arg3, arg4 );
	this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

void TestCommand1Impl::TEST_CMD_2_cmdHandler(
        FwOpcodeType opCode, //!< The opcode
        U32 cmdSeq, //!< The command sequence number
        U32 arg1, //!< The U32 command argument
        F64 arg2, //!< The F64 command argument
        I8 arg3 //!< The I8 command argument
    ) {

}
