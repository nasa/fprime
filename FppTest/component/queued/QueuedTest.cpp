// ======================================================================
// \title  QueuedTest.cpp
// \author tiffany
// \brief  cpp file for QueuedTest component implementation class
// ======================================================================


#include "QueuedTest.hpp"
#include <FpConfig.hpp>


  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  QueuedTest ::
    QueuedTest(
        const char *const compName
    ) : QueuedTestComponentBase(compName)
  {

  }

  void QueuedTest ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE msgSize,
        const NATIVE_INT_TYPE instance
    )
  {
    QueuedTestComponentBase::init(queueDepth, msgSize, instance);
  }

  QueuedTest ::
    ~QueuedTest()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void QueuedTest ::
    arrayArgsAsyncBlockPriority_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO
  }

  void QueuedTest ::
    arrayArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO
  }

  void QueuedTest ::
    arrayArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO
  }

  FormalParamArray QueuedTest ::
    arrayReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO return
  }

  FormalParamArray QueuedTest ::
    arrayReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    // TODO return
  }

  void QueuedTest ::
    enumArgsAsyncAssert_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO
  }

  void QueuedTest ::
    enumArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO
  }

  void QueuedTest ::
    enumArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO
  }

  FormalParamEnum QueuedTest ::
    enumReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO return
  }

  FormalParamEnum QueuedTest ::
    enumReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    // TODO return
  }

  void QueuedTest ::
    noArgsAsync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  void QueuedTest ::
    noArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  bool QueuedTest ::
    noArgsReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO return
  }

  bool QueuedTest ::
    noArgsReturnSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO return
  }

  void QueuedTest ::
    noArgsSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    // TODO
  }

  void QueuedTest ::
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

  void QueuedTest ::
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

  void QueuedTest ::
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

  U32 QueuedTest ::
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

  U32 QueuedTest ::
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

  void QueuedTest ::
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

  void QueuedTest ::
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

  void QueuedTest ::
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

  void QueuedTest ::
    structArgsAsyncDropPriority_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO
  }

  void QueuedTest ::
    structArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO
  }

  void QueuedTest ::
    structArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO
  }

  FormalParamStruct QueuedTest ::
    structReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    // TODO return
  }

  FormalParamStruct QueuedTest ::
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

  void QueuedTest ::
    serialAsync_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void QueuedTest ::
    serialAsyncAssert_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void QueuedTest ::
    serialAsyncBlockPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void QueuedTest ::
    serialAsyncDropPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void QueuedTest ::
    serialGuarded_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    // TODO
  }

  void QueuedTest ::
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

  void QueuedTest ::
    CMD_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
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

  void QueuedTest ::
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

  void QueuedTest ::
    CMD_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_STRUCT_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamStruct str
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_ASYNC_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
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

  void QueuedTest ::
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

  void QueuedTest ::
    CMD_ASYNC_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_ASYNC_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
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
  void QueuedTest ::
    internalArray_internalInterfaceHandler(
        const FormalParamArray& arr //!< An array
  )
  {
  }

  //! Internal interface handler for internalEnum
  void QueuedTest ::
    internalEnum_internalInterfaceHandler(
        const FormalParamEnum& en //!< An enum
  )
  {
  }

  //! Internal interface handler for internalNoArgs
  void QueuedTest ::
    internalNoArgs_internalInterfaceHandler()
  {
  }

  //! Internal interface handler for internalPrimitive
  void QueuedTest ::
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
  void QueuedTest ::
    internalString_internalInterfaceHandler(
        const Fw::InternalInterfaceString& str1, //!< A string
        const Fw::InternalInterfaceString& str2 //!< Another string
  )
  {
  }

  //! Internal interface handler for internalStruct
  void QueuedTest ::
    internalStruct_internalInterfaceHandler(
        const FormalParamStruct& str //!< A struct
  )
  {
  }

