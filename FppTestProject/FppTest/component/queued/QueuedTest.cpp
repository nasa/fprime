// ======================================================================
// \title  QueuedTest.cpp
// \author tiffany
// \brief  cpp file for QueuedTest component implementation class
// ======================================================================

#include "QueuedTest.hpp"
#include <Fw/FPrimeBasicTypes.hpp>

#include "FppTest/component/active/SerialPortIndexEnumAc.hpp"

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

QueuedTest ::QueuedTest(const char* const compName) : QueuedTestComponentBase(compName) {}

QueuedTest ::~QueuedTest() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

#define TestComponentName QueuedTest
#include "FppTest/component/common/typed.cpp"
#include "FppTest/component/common/typed_async.cpp"

// ----------------------------------------------------------------------
// Handler implementations for user-defined serial input ports
// ----------------------------------------------------------------------

void QueuedTest ::serialAsync_handler(FwIndexType portNum,             //!< The port number
                                      Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
) {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
}

void QueuedTest ::serialAsyncAssert_handler(FwIndexType portNum,             //!< The port number
                                            Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
) {
    this->serializeStatus = this->serialOut_out(SerialPortIndex::ENUM, Buffer);
}

void QueuedTest ::serialAsyncBlockPriority_handler(FwIndexType portNum,             //!< The port number
                                                   Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
) {
    this->serializeStatus = this->serialOut_out(SerialPortIndex::ARRAY, Buffer);
}

void QueuedTest ::serialAsyncDropPriority_handler(FwIndexType portNum,             //!< The port number
                                                  Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
) {
    this->serializeStatus = this->serialOut_out(SerialPortIndex::STRUCT, Buffer);
}

void QueuedTest ::serialGuarded_handler(FwIndexType portNum,             //!< The port number
                                        Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
) {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
}

void QueuedTest ::serialSync_handler(FwIndexType portNum,             //!< The port number
                                     Fw::SerializeBufferBase& Buffer  //!< The serialization buffer
) {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void QueuedTest ::CMD_ASYNC_NO_ARGS_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_ASYNC_PRIMITIVE_cmdHandler(const FwOpcodeType opCode,
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

void QueuedTest ::CMD_ASYNC_STRINGS_cmdHandler(const FwOpcodeType opCode,
                                               const U32 cmdSeq,
                                               const Fw::CmdStringArg& str1,
                                               const Fw::CmdStringArg& str2) {
    this->stringCmd.args.val1 = str1;
    this->stringCmd.args.val2 = str2;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_ASYNC_ENUM_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamEnum en) {
    this->enumCmd.args.val = en;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_ASYNC_ARRAY_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamArray arr) {
    this->arrayCmd.args.val = arr;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_ASYNC_STRUCT_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamStruct str) {
    this->structCmd.args.val = str;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_NO_ARGS_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq) {
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_PRIMITIVE_cmdHandler(const FwOpcodeType opCode,
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

void QueuedTest ::CMD_STRINGS_cmdHandler(const FwOpcodeType opCode,
                                         const U32 cmdSeq,
                                         const Fw::CmdStringArg& str1,
                                         const Fw::CmdStringArg& str2) {
    this->stringCmd.args.val1 = str1;
    this->stringCmd.args.val2 = str2;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_ENUM_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamEnum en) {
    this->enumCmd.args.val = en;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_ARRAY_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamArray arr) {
    this->arrayCmd.args.val = arr;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void QueuedTest ::CMD_STRUCT_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, FormalParamStruct str) {
    this->structCmd.args.val = str;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Internal interface handlers
// ----------------------------------------------------------------------

//! Internal interface handler for internalArray
void QueuedTest ::internalArray_internalInterfaceHandler(const FormalParamArray& arr  //!< An array
) {
    this->arrayInterface.args.val = arr;
}

//! Internal interface handler for internalEnum
void QueuedTest ::internalEnum_internalInterfaceHandler(const FormalParamEnum& en  //!< An enum
) {
    this->enumInterface.args.val = en;
}

//! Internal interface handler for internalNoArgs
void QueuedTest ::internalNoArgs_internalInterfaceHandler() {}

//! Internal interface handler for internalPrimitive
void QueuedTest ::internalPrimitive_internalInterfaceHandler(U32 u32_1,  //!< A U32
                                                             U32 u32_2,  //!< A U32
                                                             F32 f32_1,  //!< An F32
                                                             F32 f32_2,  //!< An F32
                                                             bool b1,    //!< A boolean
                                                             bool b2     //!< A boolean
) {
    this->primitiveInterface.args.val1 = u32_1;
    this->primitiveInterface.args.val2 = u32_2;
    this->primitiveInterface.args.val3 = f32_1;
    this->primitiveInterface.args.val4 = f32_2;
    this->primitiveInterface.args.val5 = b1;
    this->primitiveInterface.args.val6 = b2;
}

//! Internal interface handler for internalString
void QueuedTest ::internalString_internalInterfaceHandler(const Fw::InternalInterfaceString& str1,  //!< A string
                                                          const Fw::InternalInterfaceString& str2   //!< Another string
) {
    this->stringInterface.args.val1 = str1;
    this->stringInterface.args.val2 = str2;
}

//! Internal interface handler for internalStruct
void QueuedTest ::internalStruct_internalInterfaceHandler(const FormalParamStruct& str  //!< A struct
) {
    this->structInterface.args.val = str;
}

// ----------------------------------------------------------------------
// Overflow hook implementations for user-defined async ports interfaces
// ----------------------------------------------------------------------

void QueuedTest ::enumArgsHook_overflowHook(const FwIndexType portNum,
                                            const FormalParamEnum& en,
                                            FormalParamEnum& enRef,
                                            const FormalAliasEnum& enA,
                                            FormalAliasEnum& enARef) {
    this->enumArgsHookOverflowed_out(portNum, en, enRef, enA, enARef);
}
