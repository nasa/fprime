/*
 * TestCommand1Impl.cpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#include <Autocoders/Python/test/stress/TestCommandImpl.hpp>
#include <Fw/Types/String.hpp>
#include <cstdio>

TestCommand1Impl::TestCommand1Impl(const char* name) :  StressTest::TestCommandComponentBase(name)
{
}

void TestCommand1Impl::init(NATIVE_INT_TYPE queueDepth) {
    StressTest::TestCommandComponentBase::init(queueDepth);
}

TestCommand1Impl::~TestCommand1Impl() {
}

void TestCommand1Impl::aport_handler(FwIndexType portNum, I32 arg4, F32 arg5, U8 arg6) {
    printf("Received aport_Test_handler call with %i %f %d\n",arg4,arg5,arg6);
}

void TestCommand1Impl::aport2_handler(FwIndexType portNum, I32 arg4, F32 arg5, const Ref::Gnc::Quaternion& arg6) {
    Fw::String str;
    arg6.toString(str);
    printf("Received aport2_Test2_handler call with %i %f %s\n",arg4,arg5,str.toChar());
}

void TestCommand1Impl::TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, StressTest::TestCommandComponentBase::SomeEnum arg2) {
    const char *enum_str = "unknown";
    switch (arg2) {
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

    printf("Got command args: %d %s\n", arg1, enum_str);
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

void TestCommand1Impl::TEST_CMD_2_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2) {
    printf("Got command args: %d %f\n", arg1, arg2);
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
}

void TestCommand1Impl::printParam() {
    Fw::ParamValid valid = Fw::ParamValid::INVALID;
    const U32& prmRef = this->paramGet_someparam(valid);

    printf("Parameter is: %d %s\n",prmRef,valid==Fw::ParamValid::VALID?"VALID":"INVALID");
}

void TestCommand1Impl::genTlm(Ref::Gnc::Quaternion val) {
    this->tlmWrite_AQuat(val);
}

void TestCommand1Impl::sendEvent(I32 arg1, F32 arg2, U8 arg3) {
    printf("Sending event args %d, %f, %d\n",arg1, arg2, arg3);
    this->log_ACTIVITY_LO_SomeEvent(arg1,arg2,arg3);
}

