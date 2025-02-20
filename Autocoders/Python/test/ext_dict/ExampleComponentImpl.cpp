/*
 * ExampleComponentImpl.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/ext_dict/ExampleComponentImpl.hpp>
#include <cstdio>

namespace ExampleComponents {

    ExampleComponentImpl::ExampleComponentImpl(const char* name) : ExampleComponentBase(name) {
    }
    ExampleComponentImpl::~ExampleComponentImpl() {
    }


    void ExampleComponentImpl::init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance) {
        ExampleComponentBase::init(queueDepth,instance);
    }

    void ExampleComponentImpl::exampleInput_handler(FwIndexType portNum, I32 arg1, const ANameSpace::mytype& arg2, U8 arg3, const Example3::ExampleSerializable& arg4, AnotherExample::SomeEnum arg5) {
        //Fw::TlmString arg = "A string arg";
        // write some telemetry
        U32 chan = 0;
        this->tlmWrite_stringchan(chan);
        // call the output port
        if (this->isConnected_exampleOutput_OutputPort(0)) {
            this->exampleOutput_out(0,arg1,arg2,arg3,arg4,arg5);
        } else {
            printf("Output port not connected.\n");
        }

    }

    void ExampleComponentImpl::TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, ExampleComponentBase::CmdEnum arg2, const Fw::CmdStringArg& arg3) {
        // issue the test event with the opcode
        Fw::LogStringArg str = "TEST_CMD_1";
        this->log_ACTIVITY_HI_SomeEvent(opCode,static_cast<F32>(arg1), 0, str,ExampleComponentBase::EVENT_MEMB2);
        // write a value to a telemetry channel
        U32 chan = 12;
        this->tlmWrite_somechan(chan);
        this->cmdResponse_out(opCode,cmdSeq, Fw::CmdResponse::OK);
    }

    void ExampleComponentImpl::TEST_CMD_2_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2) {
        Fw::LogStringArg str = "TEST_CMD_2";
        this->log_ACTIVITY_HI_SomeEvent(opCode,arg2, 0, str,ExampleComponentBase::EVENT_MEMB3);
        U32 chan = 0;
        this->tlmWrite_anotherchan(chan); // <! Example output port
        this->cmdResponse_out(opCode,cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }


} /* namespace ExampleComponents */
