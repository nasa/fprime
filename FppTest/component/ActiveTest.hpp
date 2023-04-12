// ======================================================================
// \title  ActiveTest.hpp
// \author tiffany
// \brief  hpp file for ActiveTest component implementation class
// ======================================================================

#ifndef ActiveTest_HPP
#define ActiveTest_HPP

#include "FppTest/component/ActiveTestComponentAc.hpp"


  class ActiveTest :
    public ActiveTestComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object ActiveTest
      //!
      ActiveTest(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object ActiveTest
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE msgSize, /*!< The message size*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object ActiveTest
      //!
      ~ActiveTest();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for arrayArgsAsyncBlockPriority
      //!
      void arrayArgsAsyncBlockPriority_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamArray &a, /*!< 
      An array
      */
          FormalParamArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler implementation for arrayArgsGuarded
      //!
      void arrayArgsGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamArray &a, /*!< 
      An array
      */
          FormalParamArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler implementation for arrayArgsSync
      //!
      void arrayArgsSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamArray &a, /*!< 
      An array
      */
          FormalParamArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler implementation for arrayReturnGuarded
      //!
      FormalParamArray arrayReturnGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamArray &a, /*!< 
      An array
      */
          FormalParamArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler implementation for arrayReturnSync
      //!
      FormalParamArray arrayReturnSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamArray &a, /*!< 
      An array
      */
          FormalParamArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler implementation for enumArgsAsyncAssert
      //!
      void enumArgsAsyncAssert_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamEnum &en, /*!< 
      An enum
      */
          FormalParamEnum &enRef /*!< 
      An enum ref
      */
      );

      //! Handler implementation for enumArgsGuarded
      //!
      void enumArgsGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamEnum &en, /*!< 
      An enum
      */
          FormalParamEnum &enRef /*!< 
      An enum ref
      */
      );

      //! Handler implementation for enumArgsSync
      //!
      void enumArgsSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamEnum &en, /*!< 
      An enum
      */
          FormalParamEnum &enRef /*!< 
      An enum ref
      */
      );

      //! Handler implementation for enumReturnGuarded
      //!
      FormalParamEnum enumReturnGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamEnum &en, /*!< 
      An enum
      */
          FormalParamEnum &enRef /*!< 
      An enum ref
      */
      );

      //! Handler implementation for enumReturnSync
      //!
      FormalParamEnum enumReturnSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamEnum &en, /*!< 
      An enum
      */
          FormalParamEnum &enRef /*!< 
      An enum ref
      */
      );

      //! Handler implementation for noArgsAsync
      //!
      void noArgsAsync_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler implementation for noArgsGuarded
      //!
      void noArgsGuarded_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler implementation for noArgsReturnGuarded
      //!
      bool noArgsReturnGuarded_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler implementation for noArgsReturnSync
      //!
      bool noArgsReturnSync_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler implementation for noArgsSync
      //!
      void noArgsSync_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler implementation for primitiveArgsAsync
      //!
      void primitiveArgsAsync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );

      //! Handler implementation for primitiveArgsGuarded
      //!
      void primitiveArgsGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );

      //! Handler implementation for primitiveArgsSync
      //!
      void primitiveArgsSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );

      //! Handler implementation for primitiveReturnGuarded
      //!
      U32 primitiveReturnGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );

      //! Handler implementation for primitiveReturnSync
      //!
      U32 primitiveReturnSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );

      //! Handler implementation for stringArgsAsync
      //!
      void stringArgsAsync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const str80String &str80, /*!< 
      A string of size 80
      */
          str80RefString &str80Ref, 
          const str100String &str100, /*!< 
      A string of size 100
      */
          str100RefString &str100Ref 
      );

      //! Handler implementation for stringArgsGuarded
      //!
      void stringArgsGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const str80String &str80, /*!< 
      A string of size 80
      */
          str80RefString &str80Ref, 
          const str100String &str100, /*!< 
      A string of size 100
      */
          str100RefString &str100Ref 
      );

      //! Handler implementation for stringArgsSync
      //!
      void stringArgsSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const str80String &str80, /*!< 
      A string of size 80
      */
          str80RefString &str80Ref, 
          const str100String &str100, /*!< 
      A string of size 100
      */
          str100RefString &str100Ref 
      );

      //! Handler implementation for structArgsAsyncDropPriority
      //!
      void structArgsAsyncDropPriority_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamStruct &s, /*!< 
      A struct
      */
          FormalParamStruct &sRef /*!< 
      A struct ref
      */
      );

      //! Handler implementation for structArgsGuarded
      //!
      void structArgsGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamStruct &s, /*!< 
      A struct
      */
          FormalParamStruct &sRef /*!< 
      A struct ref
      */
      );

      //! Handler implementation for structArgsSync
      //!
      void structArgsSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamStruct &s, /*!< 
      A struct
      */
          FormalParamStruct &sRef /*!< 
      A struct ref
      */
      );

      //! Handler implementation for structReturnGuarded
      //!
      FormalParamStruct structReturnGuarded_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamStruct &s, /*!< 
      A struct
      */
          FormalParamStruct &sRef /*!< 
      A struct ref
      */
      );

      //! Handler implementation for structReturnSync
      //!
      FormalParamStruct structReturnSync_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamStruct &s, /*!< 
      A struct
      */
          FormalParamStruct &sRef /*!< 
      A struct ref
      */
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined serial input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for serialAsync
      //!
      void serialAsync_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

      //! Handler implementation for serialAsyncAssert
      //!
      void serialAsyncAssert_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

      //! Handler implementation for serialAsyncBlockPriority
      //!
      void serialAsyncBlockPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

      //! Handler implementation for serialAsyncDropPriority
      //!
      void serialAsyncDropPriority_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

      //! Handler implementation for serialGuarded
      //!
      void serialGuarded_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

      //! Handler implementation for serialSync
      //!
      void serialSync_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for CMD_SYNC command handler
      //! A sync command with no params
      void CMD_SYNC_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for CMD_SYNC_PRIMITIVE command handler
      //! An async command with primitive params
      void CMD_SYNC_PRIMITIVE_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U32 u32, /*!< 
          A U32
          */
          F32 f32, /*!< 
          An F32
          */
          bool b /*!< 
          A boolean
          */
      );

      //! Implementation for CMD_SYNC_STRING command handler
      //! A sync command with string params
      void CMD_SYNC_STRING_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& str1, /*!< 
          A string
          */
          const Fw::CmdStringArg& str2 /*!< 
          Another string
          */
      );

      //! Implementation for CMD_SYNC_ENUM command handler
      //! A sync command with enum params
      void CMD_SYNC_ENUM_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          FormalParamEnum en /*!< 
          An enum
          */
      );

      //! Implementation for CMD_SYNC_ARRAY command handler
      //! A sync command with array params
      void CMD_SYNC_ARRAY_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          FormalParamArray arr /*!< 
          An array
          */
      );

      //! Implementation for CMD_SYNC_STRUCT command handler
      //! A sync command with struct params
      void CMD_SYNC_STRUCT_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          FormalParamStruct str /*!< 
          A struct
          */
      );

      //! Implementation for CMD_GUARDED command handler
      //! A guarded command with no params
      void CMD_GUARDED_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for CMD_GUARDED_PRIMITIVE command handler
      //! An aguarded command with primitive params
      void CMD_GUARDED_PRIMITIVE_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U32 u32, /*!< 
          A U32
          */
          F32 f32, /*!< 
          An F32
          */
          bool b /*!< 
          A boolean
          */
      );

      //! Implementation for CMD_GUARDED_STRING command handler
      //! A guarded command with string params
      void CMD_GUARDED_STRING_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& str1, /*!< 
          A string
          */
          const Fw::CmdStringArg& str2 /*!< 
          Another string
          */
      );

      //! Implementation for CMD_GUARDED_ENUM command handler
      //! A guarded command with enum params
      void CMD_GUARDED_ENUM_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          FormalParamEnum en /*!< 
          An enum
          */
      );

      //! Implementation for CMD_GUARDED_ARRAY command handler
      //! A guarded command with array params
      void CMD_GUARDED_ARRAY_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          FormalParamArray arr /*!< 
          An array
          */
      );

      //! Implementation for CMD_GUARDED_STRUCT command handler
      //! A guarded command with struct params
      void CMD_GUARDED_STRUCT_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          FormalParamStruct str /*!< 
          A struct
          */
      );

      //! Implementation for CMD_ASYNC command handler
      //! An async command with no params
      void CMD_ASYNC_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for CMD_PRIORITY command handler
      //! A command with priority
      void CMD_PRIORITY_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for CMD_PARAMS_PRIORITY command handler
      //! A command with params and priority
      void CMD_PARAMS_PRIORITY_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U32 u32 
      );

      //! Implementation for CMD_DROP command handler
      //! A command with queue full behavior
      void CMD_DROP_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for CMD_PARAMS_PRIORITY_DROP command handler
      //! A command with params, priority, and queue full behavior
      void CMD_PARAMS_PRIORITY_DROP_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U32 u32 
      );

    // ----------------------------------------------------------------------
    // Internal interface handlers
    // ----------------------------------------------------------------------

    //! Internal Interface handler for internalArray
    //!
    void internalArray_internalInterfaceHandler(
        const FormalParamArray& arr /*!< 
          An array
          */
    );

    //! Internal Interface handler for internalEnum
    //!
    void internalEnum_internalInterfaceHandler(
        const FormalParamEnum& en /*!< 
          An enum
          */
    );

    //! Internal Interface handler for internalPrimitive
    //!
    void internalPrimitive_internalInterfaceHandler(
        U32 u32, /*!< 
          A U32
          */
        F32 f32, /*!< 
          An F32
          */
        bool b /*!< 
          A boolean
          */
    );

    //! Internal Interface handler for internalPriorityDrop
    //!
    void internalPriorityDrop_internalInterfaceHandler();

    //! Internal Interface handler for internalString
    //!
    void internalString_internalInterfaceHandler(
        const Fw::InternalInterfaceString& str1, /*!< 
          A string
          */
        const Fw::InternalInterfaceString& str2 /*!< 
          Another string
          */
    );

    //! Internal Interface handler for internalStruct
    //!
    void internalStruct_internalInterfaceHandler(
        const FormalParamStruct& str /*!< 
          A struct
          */
    );


    };


#endif
