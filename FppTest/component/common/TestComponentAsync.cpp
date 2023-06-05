#include "TestComponent.hpp"

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void TestComponent ::
    arrayArgsAsync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    this->arrayArgsOut_out(portNum, a, aRef);
  }

  void TestComponent ::
    enumArgsAsync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->enumArgsOut_out(portNum, en, enRef);
  }

  void TestComponent ::
    noArgsAsync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->noArgsOut_out(portNum);
  }

  void TestComponent ::
    primitiveArgsAsync_handler(
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
    structArgsAsync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->structArgsOut_out(portNum, s, sRef);
  }
  void TestComponent ::
    stringArgsAsync_handler(
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

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined serial input ports
  // ----------------------------------------------------------------------

  void TestComponent ::
    serialAsync_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
  }

  void TestComponent ::
    serialAsyncAssert_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void TestComponent ::
    serialAsyncBlockPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void TestComponent ::
    serialAsyncDropPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void TestComponent ::
    CMD_ASYNC_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
    CMD_ASYNC_PRIMITIVE_cmdHandler(
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

  void TestComponent ::
    CMD_ASYNC_STRINGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& str1,
        const Fw::CmdStringArg& str2
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
    CMD_ASYNC_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
    CMD_ASYNC_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void TestComponent ::
    CMD_ASYNC_STRUCT_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamStruct str
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  // ----------------------------------------------------------------------
  // Internal interface handlers
  // ----------------------------------------------------------------------

  //! Internal interface handler for internalArray
  void TestComponent ::
    internalArray_internalInterfaceHandler(
        const FormalParamArray& arr //!< An array
  )
  {
  }

  //! Internal interface handler for internalEnum
  void TestComponent ::
    internalEnum_internalInterfaceHandler(
        const FormalParamEnum& en //!< An enum
  )
  {
  }

  //! Internal interface handler for internalNoArgs
  void TestComponent ::
    internalNoArgs_internalInterfaceHandler()
  {
  }

  //! Internal interface handler for internalPrimitive
  void TestComponent ::
    internalPrimitive_internalInterfaceHandler(
        U32 u32_1, //!< A U32
        U32 u32_2, //!< A U32
        F32 f32_1, //!< An F32
        F32 f32_2, //!< An F32
        bool b1, //!< A boolean
        bool b2 //!< A boolean
  )
  {
  }

  //! Internal interface handler for internalString
  void TestComponent ::
    internalString_internalInterfaceHandler(
        const Fw::InternalInterfaceString& str1, //!< A string
        const Fw::InternalInterfaceString& str2 //!< Another string
  )
  {
  }

  //! Internal interface handler for internalStruct
  void TestComponent ::
    internalStruct_internalInterfaceHandler(
        const FormalParamStruct& str //!< A struct
  )
  {
  }
