/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/command_string/TestCommandImpl.hpp>
#include <cstdio>

TestCommand1Impl::TestCommand1Impl(const char* name) :  AcTest::TestCommandComponentBase(name)
{
}

void TestCommand1Impl::init(NATIVE_INT_TYPE queueDepth) {
    AcTest::TestCommandComponentBase::init(queueDepth);
}

TestCommand1Impl::~TestCommand1Impl() {
}

void TestCommand1Impl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {

}

void TestCommand1Impl::TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, const Fw::CmdStringArg& arg2, SomeEnum arg3) {

    const char *enum_str = "unknown";
    switch (arg3) {
        case MEMB1:
            enum_str = "MEMB1";
            break;
        case MEMB2:
            enum_str = "MEMB2";
            break;
        case MEMB3:
            enum_str = "MEMB3";
            break;
        default:
            enum_str = "INV";
            break;
    }

    printf("Got command args: %d \"%s\" %s\n", arg1, arg2.toChar(),enum_str);
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}
