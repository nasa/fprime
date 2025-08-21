// ======================================================================
// \title  PassiveTest.hpp
// \author tiffany
// \brief  hpp file for PassiveTest component implementation class
// ======================================================================

#ifndef PassiveTest_HPP
#define PassiveTest_HPP

#include "FppTest/component/passive/PassiveTestComponentAc.hpp"
#include "FppTest/component/types/FormalParamTypes.hpp"

class PassiveTest : public PassiveTestComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct PassiveTest object
    PassiveTest(const char* const compName  //!< The component name
    );

    //! Destroy PassiveTest object
    ~PassiveTest();

  private:
#include "FppTest/component/common/typed.hpp"

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined serial input ports
    // ----------------------------------------------------------------------

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

  public:
    //! Enables checking the serialization status of serial port invocations
    Fw::SerializeStatus serializeStatus;

    // Command test values
    FppTest::Types::PrimitiveParams primitiveCmd;
    FppTest::Types::CmdStringParams stringCmd;
    FppTest::Types::EnumParam enumCmd;
    FppTest::Types::ArrayParam arrayCmd;
    FppTest::Types::StructParam structCmd;
};

#endif
