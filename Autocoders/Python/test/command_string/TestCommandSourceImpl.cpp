/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/command_string/TestCommandSourceImpl.hpp>
#include <cstdio>


TestCommandSourceImpl::TestCommandSourceImpl(const char* name) : Cmd::CommandTesterComponentBase(name)
{
}

TestCommandSourceImpl::~TestCommandSourceImpl() {
}

void TestCommandSourceImpl::cmdStatusPort_handler(
        FwIndexType portNum, FwOpcodeType opCode, U32 cmdSeq,
        const Fw::CmdResponse& response) {
    this->printStatus(response);
}

void TestCommandSourceImpl::cmdRegPort_handler(FwIndexType portNum, FwOpcodeType opCode) {
    printf("Received registration for opcode %d\n",opCode);
}

void TestCommandSourceImpl::init() {
    Cmd::CommandTesterComponentBase::init();
}

void TestCommandSourceImpl::printStatus(const Fw::CmdResponse& response) {
    switch (response.e) {
        case Fw::CmdResponse::OK:
            printf("COMMAND OK\n");
            break;
        case Fw::CmdResponse::INVALID_OPCODE:
            printf("INVALID OPCODE\n");
            break;
        case Fw::CmdResponse::VALIDATION_ERROR:
            printf("VALIDATION ERROR\n");
            break;
        case Fw::CmdResponse::FORMAT_ERROR:
            printf("FORMAT ERROR\n");
            break;
        case Fw::CmdResponse::EXECUTION_ERROR:
            printf("EXECUTION ERROR\n");
            break;
        default:
            printf("Unknown status %d\n", response.e);
            break;
    }
}

void TestCommandSourceImpl::test_TEST_CMD_1(I32 arg1, const Fw::CmdStringArg& arg2, I32 arg3) {

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
return;
    // wrong value

//    argBuff.resetToStart();
//    argBuff.serialize(arg1);
//    argBuff.serialize(arg2+1);
//
//    this->cmdSendPort_FwCmd_out(0, 0x100, 1, argBuff);
//
//    // too many arguments
//    argBuff.resetToStart();
//
//    argBuff.serialize(arg1);
//    argBuff.serialize(arg2);
//    argBuff.serialize(arg2);
//
//    this->cmdSendPort_FwCmd_out(0, 0x100, 1, argBuff);
//
//    // too few arguments
//    argBuff.resetToStart();
//
//    argBuff.serialize(arg1);
//
//    this->cmdSendPort_FwCmd_out(0, 0x100, 1, argBuff);
}

