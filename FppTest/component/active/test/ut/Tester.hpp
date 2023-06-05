// ======================================================================
// \title  TestComponent/test/ut/Tester.hpp
// \author tiffany
// \brief  hpp file for TestComponent test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "FppTest/component/active/TestComponent.hpp"
#include "FppTest/component/active/SerialPortIndexEnumAc.hpp"
#include "FppTest/component/active/TypedPortIndexEnumAc.hpp"
#include "FppTest/component/common/PortTests.hpp"
#include "FppTest/component/common/CmdTests.hpp"
#include "FppTest/component/common/EventTests.hpp"
#include "FppTest/component/common/TlmTests.hpp"
#include "FppTest/types/FormalParamTypes.hpp"

  class Tester :
    public TestComponentGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:
      // Maximum size of histories storing events, telemetry, and port outputs
      static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 100;
      // Instance ID supplied to the component instance under test
      static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;
      // Queue depth supplied to component instance under test
      static const NATIVE_INT_TYPE TEST_INSTANCE_QUEUE_DEPTH = 10;

      //! Construct object Tester
      //!
      Tester();

      //! Destroy object Tester
      //!
      ~Tester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      PORT_TEST_DECLS

      CMD_TEST_DECLS

      EVENT_TEST_DECLS

      TLM_TEST_DECLS

      void testParam();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_arrayArgsOut
      //!
      void from_arrayArgsOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamArray &a, /*!< 
      An array
      */
          FormalParamArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler for from_arrayReturnOut
      //!
      FormalParamArray from_arrayReturnOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamArray &a, /*!< 
      An array
      */
          FormalParamArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler for from_cmdRegIn
      //!
      void from_cmdRegIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode /*!< 
      Command Op Code
      */
      );

      //! Handler for from_cmdResponseIn
      //!
      void from_cmdResponseIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwOpcodeType opCode, /*!< 
      Command Op Code
      */
          U32 cmdSeq, /*!< 
      Command Sequence
      */
          const Fw::CmdResponse &response /*!< 
      The command response argument
      */
      );

      //! Handler for from_enumArgsOut
      //!
      void from_enumArgsOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamEnum &en, /*!< 
      An enum
      */
          FormalParamEnum &enRef /*!< 
      An enum ref
      */
      );

      //! Handler for from_enumReturnOut
      //!
      FormalParamEnum from_enumReturnOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamEnum &en, /*!< 
      An enum
      */
          FormalParamEnum &enRef /*!< 
      An enum ref
      */
      );

      //! Handler for from_noArgsOut
      //!
      void from_noArgsOut_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler for from_noArgsReturnOut
      //!
      bool from_noArgsReturnOut_handler(
          const NATIVE_INT_TYPE portNum /*!< The port number*/
      );

      //! Handler for from_primitiveArgsOut
      //!
      void from_primitiveArgsOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );

      //! Handler for from_primitiveReturnOut
      //!
      U32 from_primitiveReturnOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 u32, 
          U32 &u32Ref, 
          F32 f32, 
          F32 &f32Ref, 
          bool b, 
          bool &bRef 
      );

      //! Handler for from_prmGetIn
      //!
      Fw::ParamValid from_prmGetIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< 
      Parameter ID
      */
          Fw::ParamBuffer &val /*!< 
      Buffer containing serialized parameter value
      */
      );

      //! Handler for from_prmGetIn
      //!
      void from_prmSetIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          FwPrmIdType id, /*!< 
      Parameter ID
      */
          Fw::ParamBuffer &val /*!< 
      Buffer containing serialized parameter value
      */
      );

      //! Handler for from_stringArgsOut
      //!
      void from_stringArgsOut_handler(
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

      //! Handler for from_structArgsOut
      //!
      void from_structArgsOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamStruct &s, /*!< 
      A struct
      */
          FormalParamStruct &sRef /*!< 
      A struct ref
      */
      );

      //! Handler for from_structReturnOut
      //!
      FormalParamStruct from_structReturnOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const FormalParamStruct &s, /*!< 
      A struct
      */
          FormalParamStruct &sRef /*!< 
      A struct ref
      */
      );

    private:

      // ----------------------------------------------------------------------
      // Handlers for serial from ports
      // ----------------------------------------------------------------------

      //! Handler for from_serialOut
      //!
      void from_serialOut_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

      //! Check successful status of a serial port invocation
      void checkSerializeStatusSuccess();

      //! Check unsuccessful status of a serial port invocation
      void checkSerializeStatusBufferEmpty();

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      TestComponent component;

      // Values returned by typed output ports
      FppTest::Types::BoolType noParamReturnVal;
      FppTest::Types::U32Type primitiveReturnVal;
      FppTest::Types::EnumType enumReturnVal;
      FppTest::Types::ArrayType arrayReturnVal;
      FppTest::Types::StructType structReturnVal;

      // Buffers from serial output ports;
      U8 primitiveData[InputPrimitiveArgsPort::SERIALIZED_SIZE];
      U8 stringData[InputStringArgsPort::SERIALIZED_SIZE]; 
      U8 enumData[InputEnumArgsPort::SERIALIZED_SIZE]; 
      U8 arrayData[InputArrayArgsPort::SERIALIZED_SIZE];
      U8 structData[InputStructArgsPort::SERIALIZED_SIZE];
      U8 serialData[SERIAL_ARGS_BUFFER_CAPACITY];

      Fw::SerialBuffer primitiveBuf;
      Fw::SerialBuffer stringBuf;
      Fw::SerialBuffer enumBuf;
      Fw::SerialBuffer arrayBuf;
      Fw::SerialBuffer structBuf;
      Fw::SerialBuffer serialBuf;

      // Command test values
      Fw::CmdResponse cmdResp;

      // Parameter test values
      FppTest::Types::BoolParam boolPrm;
      FppTest::Types::U32Param u32Prm;
      FppTest::Types::PrmStringParam stringPrm;
      FppTest::Types::EnumParam enumPrm;
      FppTest::Types::ArrayParam arrayPrm;
      FppTest::Types::StructParam structPrm;
      Fw::ParamValid prmValid;

  };

#endif
