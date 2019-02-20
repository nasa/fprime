/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/command1/TestCommandSourceImpl.hpp>
#include <stdio.h>

#if FW_OBJECT_NAMES == 1
TestCommandSourceImpl::TestCommandSourceImpl(const char* name) : Cmd::CommandTesterComponentBase(name)
#else
TestCommandSourceImpl::TestCommandSourceImpl() :
        Cmd::CommandTesterComponentBase()
#endif
{
    // TODO Auto-generated constructor stub

}

TestCommandSourceImpl::~TestCommandSourceImpl() {
    // TODO Auto-generated destructor stub
}

void TestCommandSourceImpl::cmdStatusPort_handler(
        NATIVE_INT_TYPE portNum, FwOpcodeType opCode, U32 cmdSeq,
        Fw::CommandResponse response) {
    this->printStatus(response);
}

void TestCommandSourceImpl::cmdRegPort_handler(NATIVE_INT_TYPE portNum, FwOpcodeType opCode) {
    printf("Received registration for opcode %d\n",opCode);
}

void TestCommandSourceImpl::init(void) {
    Cmd::CommandTesterComponentBase::init();
}

void TestCommandSourceImpl::printStatus(Fw::CommandResponse response) {
    switch (response) {
        case Fw::COMMAND_OK:
            printf("COMMAND OK\n");
            break;
        case Fw::COMMAND_INVALID_OPCODE:
            printf("INVALID OPCODE\n");
            break;
        case Fw::COMMAND_VALIDATION_ERROR:
            printf("VALIDATION ERROR\n");
            break;
        case Fw::COMMAND_FORMAT_ERROR:
            printf("FORMAT ERROR\n");
            break;
        case Fw::COMMAND_EXECUTION_ERROR:
            printf("EXECUTION ERROR\n");
            break;
        default:
            printf("Unknown status %d\n", response);
            break;
    }
}

void TestCommandSourceImpl::test_TEST_CMD_1(I32 arg1, F32 arg2, U8 arg3) {

    // serialize parameters
    if (!this->isConnected_cmdSendPort_OutputPort(0)) {
        printf("Test command port not connected.");
        return;
    }

    Fw::CmdArgBuffer argBuff;

    // do nominal case

    argBuff.serialize(arg1);
    argBuff.serialize(arg2);
    argBuff.serialize(arg3);

    this->cmdSendPort_out(0, 0x100, 1, argBuff);

    // too many arguments
    argBuff.resetSer();

    argBuff.serialize(arg1);
    argBuff.serialize(arg2);
    argBuff.serialize(arg3);
    argBuff.serialize(arg3);

    this->cmdSendPort_out(0, 0x100, 1, argBuff);

    // too few arguments
    argBuff.resetSer();

    argBuff.serialize(arg1);
    argBuff.serialize(arg2);

    this->cmdSendPort_out(0, 0x100, 1, argBuff);
}

