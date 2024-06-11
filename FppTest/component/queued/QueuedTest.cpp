// ======================================================================
// \title  QueuedTest.cpp
// \author tiffany
// \brief  cpp file for QueuedTest component implementation class
// ======================================================================


#include "QueuedTest.hpp"
#include <FpConfig.hpp>

#include "FppTest/component/active/SerialPortIndexEnumAc.hpp"

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
        NATIVE_INT_TYPE queueDepth,
        NATIVE_INT_TYPE msgSize,
        NATIVE_INT_TYPE instance
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
    arrayArgsAsync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    this->arrayArgsOut_out(portNum, a, aRef);
  }

  void QueuedTest ::
    enumArgsAsync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->enumArgsOut_out(portNum, en, enRef);
  }

  void QueuedTest ::
    noArgsAsync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->noArgsOut_out(portNum);
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

  void QueuedTest ::
    structArgsAsync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->structArgsOut_out(portNum, s, sRef);
  }
  void QueuedTest ::
    stringArgsAsync_handler(
        const NATIVE_INT_TYPE portNum,
        const Fw::StringBase &str80,
        Fw::StringBase &str80Ref,
        const Fw::StringBase &str100,
        Fw::StringBase &str100Ref
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

  void QueuedTest ::
    arrayArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    this->arrayArgsOut_out(portNum, a, aRef);
  }

  void QueuedTest ::
    arrayArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    this->arrayArgsOut_out(portNum, a, aRef);
  }

  FormalParamArray QueuedTest ::
    arrayReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    return this->arrayReturnOut_out(portNum, a, aRef);
  }

  FormalParamArray QueuedTest ::
    arrayReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamArray &a,
        FormalParamArray &aRef
    )
  {
    return this->arrayReturnOut_out(portNum, a, aRef);
  }

  void QueuedTest :: 
    cmdOut_handler(
        NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer& args
    )
  {
  }

  void QueuedTest ::
    enumArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->enumArgsOut_out(portNum, en, enRef);
  }

  void QueuedTest ::
    enumArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    this->enumArgsOut_out(portNum, en, enRef);
  }

  FormalParamEnum QueuedTest ::
    enumReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    return this->enumReturnOut_out(portNum, en, enRef);
  }

  FormalParamEnum QueuedTest ::
    enumReturnSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamEnum &en,
        FormalParamEnum &enRef
    )
  {
    return this->enumReturnOut_out(portNum, en, enRef);
  }

  void QueuedTest ::
    noArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->noArgsOut_out(portNum);
  }

  bool QueuedTest ::
    noArgsReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    return this->noArgsReturnOut_out(portNum);
  }

  bool QueuedTest ::
    noArgsReturnSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    return this->noArgsReturnOut_out(portNum);
  }

  void QueuedTest ::
    noArgsSync_handler(
        const NATIVE_INT_TYPE portNum
    )
  {
    this->noArgsOut_out(portNum);
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

  void QueuedTest ::
    stringArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const Fw::StringBase &str80,
        Fw::StringBase &str80Ref,
        const Fw::StringBase &str100,
        Fw::StringBase &str100Ref
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

  void QueuedTest ::
    stringArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const Fw::StringBase &str80,
        Fw::StringBase &str80Ref,
        const Fw::StringBase &str100,
        Fw::StringBase &str100Ref
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

  void QueuedTest ::
    structArgsGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->structArgsOut_out(portNum, s, sRef);
  }

  void QueuedTest ::
    structArgsSync_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    this->structArgsOut_out(portNum, s, sRef);
  }

  FormalParamStruct QueuedTest ::
    structReturnGuarded_handler(
        const NATIVE_INT_TYPE portNum,
        const FormalParamStruct &s,
        FormalParamStruct &sRef
    )
  {
    return this->structReturnOut_out(portNum, s, sRef);
  }

  FormalParamStruct QueuedTest ::
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

  void QueuedTest ::
    serialAsync_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
  }

  void QueuedTest ::
    serialAsyncAssert_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(SerialPortIndex::ENUM, Buffer);
  }

  void QueuedTest ::
    serialAsyncBlockPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(SerialPortIndex::ARRAY, Buffer);
  }

  void QueuedTest ::
    serialAsyncDropPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(SerialPortIndex::STRUCT, Buffer);
  }

  void QueuedTest ::
    serialGuarded_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    this->serializeStatus = this->serialOut_out(portNum, Buffer);
  }

  void QueuedTest ::
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

  void QueuedTest ::
    CMD_ASYNC_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
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
    this->primitiveCmd.args.val1 = u32_1;
    this->primitiveCmd.args.val2 = u32_2;
    this->primitiveCmd.args.val3 = f32_1;
    this->primitiveCmd.args.val4 = f32_2;
    this->primitiveCmd.args.val5 = b1;
    this->primitiveCmd.args.val6 = b2;

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
    this->stringCmd.args.val1 = str1;
    this->stringCmd.args.val2 = str2;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_ASYNC_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    this->enumCmd.args.val = en;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_ASYNC_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    this->arrayCmd.args.val = arr;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_ASYNC_STRUCT_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamStruct str
    )
  {
    this->structCmd.args.val = str;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_NO_ARGS_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
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
    this->primitiveCmd.args.val1 = u32_1;
    this->primitiveCmd.args.val2 = u32_2;
    this->primitiveCmd.args.val3 = f32_1;
    this->primitiveCmd.args.val4 = f32_2;
    this->primitiveCmd.args.val5 = b1;
    this->primitiveCmd.args.val6 = b2;

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
    this->stringCmd.args.val1 = str1;
    this->stringCmd.args.val2 = str2;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_ENUM_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamEnum en
    )
  {
    this->enumCmd.args.val = en;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_ARRAY_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamArray arr
    )
  {
    this->arrayCmd.args.val = arr;

    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }

  void QueuedTest ::
    CMD_STRUCT_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        FormalParamStruct str
    )
  {
    this->structCmd.args.val = str;

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
    this->arrayInterface.args.val = arr;
  }

  //! Internal interface handler for internalEnum
  void QueuedTest ::
    internalEnum_internalInterfaceHandler(
        const FormalParamEnum& en //!< An enum
  )
  {
    this->enumInterface.args.val = en;
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
    this->primitiveInterface.args.val1 = u32_1;
    this->primitiveInterface.args.val2 = u32_2;
    this->primitiveInterface.args.val3 = f32_1;
    this->primitiveInterface.args.val4 = f32_2;
    this->primitiveInterface.args.val5 = b1;
    this->primitiveInterface.args.val6 = b2;
  }

  //! Internal interface handler for internalString
  void QueuedTest ::
    internalString_internalInterfaceHandler(
        const Fw::InternalInterfaceString& str1, //!< A string
        const Fw::InternalInterfaceString& str2 //!< Another string
  )
  {
    this->stringInterface.args.val1 = str1;
    this->stringInterface.args.val2 = str2;
  }

  //! Internal interface handler for internalStruct
  void QueuedTest ::
    internalStruct_internalInterfaceHandler(
        const FormalParamStruct& str //!< A struct
  )
  {
    this->structInterface.args.val = str;
  }

