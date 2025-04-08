/*
 * ExampleComponentImpl.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/templates/ExampleComponentImpl.hpp>
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
        Fw::TlmString arg = "A string arg";
        // write some telemetry
        this->tlmWrite_stringchan(arg);
        // call the output port
        if (this->isConnected_exampleOutput_OutputPort(0)) {
            this->exampleOutput_out(0,arg1,arg2,arg3,arg4,arg5);
        } else {
            printf("Output port not connected.\n");
        }

    }

    SomeOtherNamespace::AnotherEnum ExampleComponentImpl::anotherInput_handler(FwIndexType portNum, I32 arg1, F64 arg2, SomeOtherNamespace::SomeEnum arg3) {

    	return SomeOtherNamespace::MEMB1;
    }


    void ExampleComponentImpl::TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, ExampleComponentBase::CmdEnum arg2, const Fw::CmdStringArg& arg3) {
        // issue the test event with the opcode
        Fw::LogStringArg str = "TEST_CMD_1";
        this->log_ACTIVITY_HI_SomeEvent(opCode, static_cast<F32>(arg1), Example4::Example2(2.0,3.0,4,5), str,ExampleComponentBase::EVENT_MEMB2);
        // write a value to a telemetry channel
        U32 chan = 12;
        this->tlmWrite_somechan(chan);
        this->cmdResponse_out(opCode,cmdSeq, Fw::CmdResponse::OK);
    }

    void ExampleComponentImpl::TEST_CMD_2_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2) {
        Fw::LogStringArg str = "TEST_CMD_2";
        this->log_ACTIVITY_HI_SomeEvent(opCode,arg2, Example4::Example2(6.0,7.0,8,9), str,ExampleComponentBase::EVENT_MEMB3);
        Example4::Example2 ex(10.0,11.0,12,13);
        this->tlmWrite_anotherchan(ex); // <! Example output port
        this->cmdResponse_out(opCode,cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }

    void ExampleComponentImpl::parameterUpdated(FwPrmIdType id) {
        printf("Parameter ID %d was updated!\n",id);
    }

    void ExampleComponentImpl::test_internalInterfaceHandler(I32 arg1, F32 arg2, U8 arg3) {

    }

    void ExampleComponentImpl::test2_internalInterfaceHandler(I32 arg1, SomeEnum arg2, const Fw::InternalInterfaceString& arg3, const Example4::Example2& arg4) {

    }


} /* namespace ExampleComponents */
