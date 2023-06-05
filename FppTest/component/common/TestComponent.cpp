// ======================================================================
// \title  TestComponent.cpp
// \author tiffany
// \brief  cpp file for TestComponent component implementation class
// ======================================================================


#include "TestComponent.hpp"
#include <FpConfig.hpp>


  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestComponent ::
    TestComponent(
        const char *const compName
    ) : TestComponentComponentBase(compName)
  {

  }

  void TestComponent ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE msgSize,
        const NATIVE_INT_TYPE instance
    )
  {
    TestComponentComponentBase::init(queueDepth, msgSize, instance);
  }

  TestComponent ::
    ~TestComponent()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void TestComponent ::
    arrayArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    this->arrayArgsOut_out(portNum, a, aRef);
  }

  void TestComponent ::
    arrayArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    this->arrayArgsOut_out(portNum, a, aRef);
  }

  FormalParamArray TestComponent ::
    arrayReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    return this->arrayReturnOut_out(portNum, a, aRef);
  }

  FormalParamArray TestComponent ::
    arrayReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    return this->arrayReturnOut_out(portNum, a, aRef);
  }

  void TestComponent :: 
    cmdOut_handler(
        NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer& args
    )
  {
  }

  void TestComponent ::
    enumArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->enumArgsOut_out(portNum, en, enRef);
  }

  void TestComponent ::
    enumArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->enumArgsOut_out(portNum, en, enRef);
  }

  FormalParamEnum TestComponent ::
    enumReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    return this->enumReturnOut_out(portNum, en, enRef);
  }

  FormalParamEnum TestComponent ::
    enumReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    return this->enumReturnOut_out(portNum, en, enRef);
  }

  void TestComponent ::
    noArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->noArgsOut_out(portNum);
  }

  bool TestComponent ::
    noArgsReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    return this->noArgsReturnOut_out(portNum);
  }

  bool TestComponent ::
    noArgsReturnSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    return this->noArgsReturnOut_out(portNum);
  }

  void TestComponent ::
    noArgsSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->noArgsOut_out(portNum);
  }

  void TestComponent ::
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
    this->primitiveArgsOut_out(
      portNum, 
      u32, 
      u32Ref, 
      f32, 
      f32Ref, 
      b, 
      bRef
    );
  }

  void TestComponent ::
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
    this->primitiveArgsOut_out(
      portNum, 
      u32, 
      u32Ref, 
      f32, 
      f32Ref, 
      b, 
      bRef
    );
  }

  U32 TestComponent ::
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
    return this->primitiveReturnOut_out(
      portNum, 
      u32, 
      u32Ref, 
      f32, 
      f32Ref, 
      b, 
      bRef
    );
  }

  U32 TestComponent ::
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
    return this->primitiveReturnOut_out(
      portNum, 
      u32, 
      u32Ref, 
      f32, 
      f32Ref, 
      b, 
      bRef
    );
  }

  void TestComponent ::
    stringArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    this->stringArgsOut_out(
      portNum,
      str80,
      str80Ref,
      str100,
      str100Ref
    );
  }

  void TestComponent ::
    stringArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    this->stringArgsOut_out(
      portNum,
      str80,
      str80Ref,
      str100,
      str100Ref
    );
  }

  void TestComponent ::
    structArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->structArgsOut_out(portNum, s, sRef);
  }

  void TestComponent ::
    structArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->structArgsOut_out(portNum, s, sRef);
  }

  FormalParamStruct TestComponent ::
    structReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    return this->structReturnOut_out(portNum, s, sRef);
  }

  FormalParamStruct TestComponent ::
    structReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    return this->structReturnOut_out(portNum, s, sRef);
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined serial input ports
  // ----------------------------------------------------------------------

  void TestComponent ::
    serialGuarded_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
  }

  void TestComponent ::
    serialSync_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void TestComponent ::
    CMD_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
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
    this->primitiveCmd.args.val1 = u32_1;
    this->primitiveCmd.args.val2 = u32_2;
    this->primitiveCmd.args.val3 = f32_1;
    this->primitiveCmd.args.val4 = f32_2;
    this->primitiveCmd.args.val5 = b1;
    this->primitiveCmd.args.val6 = b2;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
    CMD_STRINGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& str1,
        const Fw::CmdStringArg& str2
    )
  {
    this->stringCmd.args.val1 = str1;
    this->stringCmd.args.val2 = str2;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
    CMD_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    this->enumCmd.args.val = en;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
    CMD_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    this->arrayCmd.args.val = arr;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
    CMD_STRUCT_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamStruct str
    )
  {
    this->structCmd.args.val = str;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

