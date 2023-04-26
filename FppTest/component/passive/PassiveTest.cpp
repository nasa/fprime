// ======================================================================
// \title  PassiveTest.cpp
// \author tiffany
// \brief  cpp file for PassiveTest component implementation class
// ======================================================================


#include "PassiveTest.hpp"
#include <FpConfig.hpp>


  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  PassiveTest ::
    PassiveTest(
        const char *const compName
    ) : PassiveTestComponentBase(compName)
  {

  }

  void PassiveTest ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    PassiveTestComponentBase::init(instance);
  }

  PassiveTest ::
    ~PassiveTest()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void PassiveTest ::
    arrayArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO
  }

  void PassiveTest ::
    arrayArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO
  }

  FormalParamArray PassiveTest ::
    arrayReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO return
  }

  FormalParamArray PassiveTest ::
    arrayReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO return
  }

  void PassiveTest ::
    enumArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO
  }

  void PassiveTest ::
    enumArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO
  }

  FormalParamEnum PassiveTest ::
    enumReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO return
  }

  FormalParamEnum PassiveTest ::
    enumReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO return
  }

  void PassiveTest ::
    noArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  bool PassiveTest ::
    noArgsReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO return
  }

  bool PassiveTest ::
    noArgsReturnSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO return
  }

  void PassiveTest ::
    noArgsSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  void PassiveTest ::
    primitiveArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    // TODO
  }

  void PassiveTest ::
    primitiveArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    // TODO
  }

  U32 PassiveTest ::
    primitiveReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    // TODO return
  }

  U32 PassiveTest ::
    primitiveReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        U32 u32,
        U32 &u32Ref,
        F32 f32,
        F32 &f32Ref,
        bool b,
        bool &bRef
    )
  {
    // TODO return
  }

  void PassiveTest ::
    stringArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    // TODO
  }

  void PassiveTest ::
    stringArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    // TODO
  }

  void PassiveTest ::
    structArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO
  }

  void PassiveTest ::
    structArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO
  }

  FormalParamStruct PassiveTest ::
    structReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO return
  }

  FormalParamStruct PassiveTest ::
    structReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO return
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined serial input ports
  // ----------------------------------------------------------------------

  void PassiveTest ::
    serialGuarded_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void PassiveTest ::
    serialSync_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void PassiveTest ::
    CMD_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void PassiveTest ::
    CMD_PRIMITIVE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 u32_1,
        U32 u32_2,
        F32 f32_1,
        F32 f32_2,
        bool b1,
        bool b2
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void PassiveTest ::
    CMD_STRINGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& str1,
        const Fw::CmdStringArg& str2
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void PassiveTest ::
    CMD_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void PassiveTest ::
    CMD_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void PassiveTest ::
    CMD_STRUCT_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamStruct str
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

