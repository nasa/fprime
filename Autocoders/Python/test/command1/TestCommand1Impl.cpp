/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/command1/TestCommand1Impl.hpp>
#include <cstdio>

TestCommand1Impl::TestCommand1Impl(const char* name) : Test1ComponentBase(name)
{
    // TODO Auto-generated constructor stub

}

TestCommand1Impl::~TestCommand1Impl() {
    // TODO Auto-generated destructor stub
}

void TestCommand1Impl::init() {
    Test1ComponentBase::init();
}

void TestCommand1Impl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {
}

void TestCommand1Impl::TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2, U8 arg3) {
	printf("Got command args: %d %f %d\n", arg1, arg2, arg3 );
	this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}
