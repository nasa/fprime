// ======================================================================
// \title  ActiveTest.cpp
// \author tiffany
// \brief  cpp file for ActiveTest component implementation class
// ======================================================================


#include "ActiveTest.hpp"
#include <FpConfig.hpp>


  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ActiveTest ::
    ActiveTest(
        const char *const compName
    ) : ActiveTestComponentBase(compName)
  {

  }

  void ActiveTest ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE msgSize,
        const NATIVE_INT_TYPE instance
    )
  {
    ActiveTestComponentBase::init(queueDepth, msgSize, instance);
  }

  ActiveTest ::
    ~ActiveTest()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void ActiveTest ::
    arrayArgsAsyncBlockPriority_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO
  }

  void ActiveTest ::
    arrayArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO
  }

  void ActiveTest ::
    arrayArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO
  }

  FormalParamArray ActiveTest ::
    arrayReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO return
  }

  FormalParamArray ActiveTest ::
    arrayReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO return
  }

  void ActiveTest ::
    enumArgsAsyncAssert_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO
  }

  void ActiveTest ::
    enumArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO
  }

  void ActiveTest ::
    enumArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO
  }

  FormalParamEnum ActiveTest ::
    enumReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO return
  }

  FormalParamEnum ActiveTest ::
    enumReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO return
  }

  void ActiveTest ::
    noArgsAsync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  void ActiveTest ::
    noArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  bool ActiveTest ::
    noArgsReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO return
  }

  bool ActiveTest ::
    noArgsReturnSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO return
  }

  void ActiveTest ::
    noArgsSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  void ActiveTest ::
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
    // TODO
  }

  void ActiveTest ::
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

  void ActiveTest ::
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

  U32 ActiveTest ::
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

  U32 ActiveTest ::
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

  void ActiveTest ::
    stringArgsAsync_handler(
        const NATIVE_INT_TYPE portNum,
        const str80String &str80,
        str80RefString &str80Ref,
        const str100String &str100,
        str100RefString &str100Ref
    )
  {
    // TODO
  }

  void ActiveTest ::
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

  void ActiveTest ::
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

  void ActiveTest ::
    structArgsAsyncDropPriority_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO
  }

  void ActiveTest ::
    structArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO
  }

  void ActiveTest ::
    structArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO
  }

  FormalParamStruct ActiveTest ::
    structReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO return
  }

  FormalParamStruct ActiveTest ::
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

  void ActiveTest ::
    serialAsync_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void ActiveTest ::
    serialAsyncAssert_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void ActiveTest ::
    serialAsyncBlockPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void ActiveTest ::
    serialAsyncDropPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void ActiveTest ::
    serialGuarded_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void ActiveTest ::
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

  void ActiveTest ::
    CMD_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void ActiveTest ::
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

  void ActiveTest ::
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

  void ActiveTest ::
    CMD_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void ActiveTest ::
    CMD_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void ActiveTest ::
    CMD_STRUCT_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamStruct str
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void ActiveTest ::
    CMD_ASYNC_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void ActiveTest ::
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

  void ActiveTest ::
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

  void ActiveTest ::
    CMD_ASYNC_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void ActiveTest ::
    CMD_ASYNC_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void ActiveTest ::
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
  void ActiveTest ::
    internalArray_internalInterfaceHandler(
        const FormalParamArray& arr //!< An array
  )
  {
  }

  //! Internal interface handler for internalEnum
  void ActiveTest ::
    internalEnum_internalInterfaceHandler(
        const FormalParamEnum& en //!< An enum
  )
  {
  }

  //! Internal interface handler for internalNoArgs
  void ActiveTest ::
    internalNoArgs_internalInterfaceHandler()
  {
  }

  //! Internal interface handler for internalPrimitive
  void ActiveTest ::
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
  void ActiveTest ::
    internalString_internalInterfaceHandler(
        const Fw::InternalInterfaceString& str1, //!< A string
        const Fw::InternalInterfaceString& str2 //!< Another string
  )
  {
  }

  //! Internal interface handler for internalStruct
  void ActiveTest ::
    internalStruct_internalInterfaceHandler(
        const FormalParamStruct& str //!< A struct
  )
  {
  }

