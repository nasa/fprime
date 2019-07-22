#include <Autocoders/Python/test/testgen/TestComponentComponentImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;

namespace TestGen {

    TestComponentComponentImpl::TestComponentComponentImpl(const char *compName) : TestComponentComponentBase(compName) {
    }

    TestComponentComponentImpl::~TestComponentComponentImpl(void) {

    }

    void TestComponentComponentImpl::init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance) {
        TestComponentComponentBase::init(instance);
    }

    void TestComponentComponentImpl::aport_handler(NATIVE_INT_TYPE portNum,I32 arg1, F32 arg2, U8 arg3) {
        printf("%s Invoked aport_handler(%d, %d, %d, %f, %d);\n", this->getObjName(), portNum, arg1, arg2, arg3);
        this->bportOut_out(0, arg1, arg2, arg3);
    }
    
    void TestComponentComponentImpl::bport_handler(NATIVE_INT_TYPE portNum, I32 arg1, F32 arg2, U8 arg3) {
        printf("%s Invoked bport_handler(%d, %d, %d, %f, %d);\n", this->getObjName(), portNum, arg1, arg2, arg3);
    }
    
    void TEST_CMD_1_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, I32 arg1, F32 arg2, U8 arg3) {
        printf("TEST_CMD_1 Invoked");
    }
};
