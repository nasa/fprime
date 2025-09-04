// ======================================================================
// \title  QueuedTest.hpp
// \author tiffany
// \brief  hpp file for QueuedTest component implementation class
// ======================================================================

#ifndef QueuedTest_HPP
#define QueuedTest_HPP

#include "FppTest/component/queued/QueuedTestComponentAc.hpp"
#include "FppTest/component/types/FormalParamTypes.hpp"

class QueuedTest : public QueuedTestComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct QueuedTest object
    QueuedTest(const char* const compName  //!< The component name
    );

    //! Destroy QueuedTest object
    ~QueuedTest();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

#define HAS_ASYNC
#include "FppTest/component/common/typed.hpp"
#include "FppTest/component/common/typed_async.hpp"

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined serial input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for serialAsync
    void serialAsync_handler(FwIndexType portNum,             //!< The port number
                             Fw::SerializeBufferBase& buffer  //!< The serialization buffer
                             ) override;

    //! Handler implementation for serialAsyncAssert
    void serialAsyncAssert_handler(FwIndexType portNum,             //!< The port number
                                   Fw::SerializeBufferBase& buffer  //!< The serialization buffer
                                   ) override;

    //! Handler implementation for serialAsyncBlockPriority
    void serialAsyncBlockPriority_handler(FwIndexType portNum,             //!< The port number
                                          Fw::SerializeBufferBase& buffer  //!< The serialization buffer
                                          ) override;

    //! Handler implementation for serialAsyncDropPriority
    void serialAsyncDropPriority_handler(FwIndexType portNum,             //!< The port number
                                         Fw::SerializeBufferBase& buffer  //!< The serialization buffer
                                         ) override;

    //! Handler implementation for serialGuarded
    void serialGuarded_handler(FwIndexType portNum,             //!< The port number
                               Fw::SerializeBufferBase& buffer  //!< The serialization buffer
                               ) override;

    //! Handler implementation for serialSync
    void serialSync_handler(FwIndexType portNum,             //!< The port number
                            Fw::SerializeBufferBase& buffer  //!< The serialization buffer
                            ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command CMD_NO_ARGS
    void CMD_NO_ARGS_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                U32 cmdSeq            //!< The command sequence number
                                ) override;

    //! Handler implementation for command CMD_PRIMITIVE
    void CMD_PRIMITIVE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                  U32 cmdSeq,           //!< The command sequence number
                                  U32 u32_1,            //!< A U32
                                  U32 u32_2,            //!< A U32
                                  F32 f32_1,            //!< An F32
                                  F32 f32_2,            //!< An F32
                                  bool b1,              //!< A boolean
                                  bool b2               //!< A boolean
                                  ) override;

    //! Handler implementation for command CMD_STRINGS
    void CMD_STRINGS_cmdHandler(FwOpcodeType opCode,           //!< The opcode
                                U32 cmdSeq,                    //!< The command sequence number
                                const Fw::CmdStringArg& str1,  //!< A string
                                const Fw::CmdStringArg& str2   //!< Another string
                                ) override;

    //! Handler implementation for command CMD_ENUM
    void CMD_ENUM_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                             U32 cmdSeq,           //!< The command sequence number
                             FormalParamEnum en    //!< An enum
                             ) override;

    //! Handler implementation for command CMD_ARRAY
    void CMD_ARRAY_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                              U32 cmdSeq,           //!< The command sequence number
                              FormalParamArray arr  //!< An array
                              ) override;

    //! Handler implementation for command CMD_STRUCT
    void CMD_STRUCT_cmdHandler(FwOpcodeType opCode,   //!< The opcode
                               U32 cmdSeq,            //!< The command sequence number
                               FormalParamStruct str  //!< A struct
                               ) override;

    //! Handler implementation for command CMD_ASYNC_NO_ARGS
    void CMD_ASYNC_NO_ARGS_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                      U32 cmdSeq            //!< The command sequence number
                                      ) override;

    //! Handler implementation for command CMD_ASYNC_PRIMITIVE
    void CMD_ASYNC_PRIMITIVE_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                        U32 cmdSeq,           //!< The command sequence number
                                        U32 u32_1,            //!< A U32
                                        U32 u32_2,            //!< A U32
                                        F32 f32_1,            //!< An F32
                                        F32 f32_2,            //!< An F32
                                        bool b1,              //!< A boolean
                                        bool b2               //!< A boolean
                                        ) override;

    //! Handler implementation for command CMD_ASYNC_STRINGS
    void CMD_ASYNC_STRINGS_cmdHandler(FwOpcodeType opCode,           //!< The opcode
                                      U32 cmdSeq,                    //!< The command sequence number
                                      const Fw::CmdStringArg& str1,  //!< A string
                                      const Fw::CmdStringArg& str2   //!< Another string
                                      ) override;

    //! Handler implementation for command CMD_ASYNC_ENUM
    void CMD_ASYNC_ENUM_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                   U32 cmdSeq,           //!< The command sequence number
                                   FormalParamEnum en    //!< An enum
                                   ) override;

    //! Handler implementation for command CMD_ASYNC_ARRAY
    void CMD_ASYNC_ARRAY_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                    U32 cmdSeq,           //!< The command sequence number
                                    FormalParamArray arr  //!< An array
                                    ) override;

    //! Handler implementation for command CMD_ASYNC_STRUCT
    void CMD_ASYNC_STRUCT_cmdHandler(FwOpcodeType opCode,   //!< The opcode
                                     U32 cmdSeq,            //!< The command sequence number
                                     FormalParamStruct str  //!< A struct
                                     ) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined internal interfaces
    // ----------------------------------------------------------------------

    //! Handler implementation for internalArray
    void internalArray_internalInterfaceHandler(const FormalParamArray& arr  //!< An array
                                                ) override;

    //! Handler implementation for internalEnum
    void internalEnum_internalInterfaceHandler(const FormalParamEnum& en  //!< An enum
                                               ) override;

    //! Handler implementation for internalNoArgs
    void internalNoArgs_internalInterfaceHandler() override;

    //! Handler implementation for internalPrimitive
    void internalPrimitive_internalInterfaceHandler(U32 u32_1,  //!< A U32
                                                    U32 u32_2,  //!< A U32
                                                    F32 f32_1,  //!< An F32
                                                    F32 f32_2,  //!< An F32
                                                    bool b1,    //!< A boolean
                                                    bool b2     //!< A boolean
                                                    ) override;

    //! Handler implementation for internalString
    void internalString_internalInterfaceHandler(const Fw::InternalInterfaceString& str1,  //!< A string
                                                 const Fw::InternalInterfaceString& str2   //!< Another string
                                                 ) override;

    //! Handler implementation for internalStruct
    void internalStruct_internalInterfaceHandler(const FormalParamStruct& str  //!< A struct
                                                 ) override;

  private:
    // ----------------------------------------------------------------------
    // Overflow hook implementations for user-defined async ports interfaces
    // ----------------------------------------------------------------------

    //! Overflow hook implementation for enumArgsOverflow
    void enumArgsHook_overflowHook(FwIndexType portNum,         //!< The port number
                                   const FormalParamEnum& en,   //!< An enum
                                   FormalParamEnum& enRef,      //!< An enum ref
                                   const FormalAliasEnum& enA,  //!< An enum alias
                                   FormalAliasEnum& enARef      //!< An enum alias ref
                                   ) override;

  public:
    //! Enables checking the serialization status of serial port invocations
    Fw::SerializeStatus serializeStatus;

    // Command test values
    FppTest::Types::PrimitiveParams primitiveCmd;
    FppTest::Types::CmdStringParams stringCmd;
    FppTest::Types::EnumParam enumCmd;
    FppTest::Types::ArrayParam arrayCmd;
    FppTest::Types::StructParam structCmd;

    // Internal interface test values
    FppTest::Types::PrimitiveParams primitiveInterface;
    FppTest::Types::InternalInterfaceStringParams stringInterface;
    FppTest::Types::EnumParam enumInterface;
    FppTest::Types::ArrayParam arrayInterface;
    FppTest::Types::StructParam structInterface;
};

#endif
