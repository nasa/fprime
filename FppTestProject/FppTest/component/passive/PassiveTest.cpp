// ======================================================================
// \title  PassiveTest.cpp
// \author tiffany
// \brief  cpp file for PassiveTest component implementation class
// ======================================================================

#include "PassiveTest.hpp"
#include <Fw/FPrimeBasicTypes.hpp>

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

PassiveTest ::PassiveTest(const char* const compName) : PassiveTestComponentBase(compName) {}

PassiveTest ::~PassiveTest() {}

#define TestComponentName PassiveTest
#include "FppTest/component/common/typed.cpp"

// ----------------------------------------------------------------------
// Handler implementations for user-defined serial input ports
// ----------------------------------------------------------------------

void PassiveTest ::serialGuarded_handler(FwIndexType portNum,             //!< The port number
                                         Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
) {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
}

void PassiveTest ::serialSync_handler(FwIndexType portNum,             //!< The port number
                                      Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
) {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void PassiveTest ::CMD_NO_ARGS_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void PassiveTest ::CMD_PRIMITIVE_cmdHandler(const FwOpcodeType opCode,
                                            const U32 cmdSeq,
                                            U32 u32_1,
                                            U32 u32_2,
                                            F32 f32_1,
                                            F32 f32_2,
                                            bool b1,
                                            bool b2) {
    this->primitiveCmd.args.val1 = u32_1;
    this->primitiveCmd.args.val2 = u32_2;
    this->primitiveCmd.args.val3 = f32_1;
    this->primitiveCmd.args.val4 = f32_2;
    this->primitiveCmd.args.val5 = b1;
    this->primitiveCmd.args.val6 = b2;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void PassiveTest ::CMD_STRINGS_cmdHandler(const FwOpcodeType opCode,
                                          const U32 cmdSeq,
                                          const Fw::CmdStringArg& str1,
                                          const Fw::CmdStringArg& str2) {
    this->stringCmd.args.val1 = str1;
    this->stringCmd.args.val2 = str2;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void PassiveTest ::CMD_ENUM_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamEnum en) {
    this->enumCmd.args.val = en;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void PassiveTest ::CMD_ARRAY_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamArray arr) {
    this->arrayCmd.args.val = arr;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void PassiveTest ::CMD_STRUCT_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamStruct str) {
    this->structCmd.args.val = str;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}
