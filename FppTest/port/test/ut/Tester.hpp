// ======================================================================
// \title  Tester.hpp
// \author T. Chieu
// \brief  hpp file for Example test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "Fw/Types/SerialBuffer.hpp"

#include "GTestBase.hpp"
#include "FppTest/port/Example.hpp"
#include "FppTest/types/FormalParamTypes.hpp"

#include "FppTest/types/PrimitiveArgsPortAc.hpp"
#include "FppTest/types/StringArgsPortAc.hpp"
#include "FppTest/types/EnumArgsPortAc.hpp"
#include "FppTest/types/ArrayArgsPortAc.hpp"
#include "FppTest/types/StructArgsPortAc.hpp"
#include "FppTest/port/TypedPortIndexEnumAc.hpp"
#include "FppTest/port/SerialPortIndexEnumAc.hpp"

  class Tester :
    public ExampleGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

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

      //! To do
      //!
      void toDo();

    public:

      // ----------------------------------------------------------------------
      // Invoke typed input ports
      // ----------------------------------------------------------------------

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::NoParams& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::PrimitiveParams& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::PortStringParams& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::EnumParams& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::ArrayParams& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::StructParams& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::SerialParam& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::NoParamReturn& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::PrimitiveReturn& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::EnumReturn& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::ArrayReturn& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::StructReturn& port
      );

      // ----------------------------------------------------------------------
      // Invoke serial input ports
      // ----------------------------------------------------------------------

      void invoke_serial(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::NoParams& port
      );

      void invoke_serial(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::PrimitiveParams& port
      );

      void invoke_serial(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::PortStringParams& port
      );

      void invoke_serial(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::EnumParams& port
      );

      void invoke_serial(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::ArrayParams& port
      );

      void invoke_serial(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::StructParams& port
      );

      void invoke_serial(
          NATIVE_INT_TYPE portNum,
          FppTest::Types::SerialParam& port
      );

      // ----------------------------------------------------------------------
      // Check history of typed output ports
      // ----------------------------------------------------------------------

      void check_history(
          FppTest::Types::NoParams& port
      );

      void check_history(
          FppTest::Types::PrimitiveParams& port
      );

      void check_history(
          FppTest::Types::PortStringParams& port
      );

      void check_history(
          FppTest::Types::EnumParams& port
      );

      void check_history(
          FppTest::Types::ArrayParams& port
      );

      void check_history(
          FppTest::Types::StructParams& port
      );

      void check_history(
          FppTest::Types::NoParamReturn& port
      );
      
      void check_history(
          FppTest::Types::PrimitiveReturn& port
      );

      void check_history(
          FppTest::Types::EnumReturn& port
      );

      void check_history(
          FppTest::Types::ArrayReturn& port
      );

      void check_history(
          FppTest::Types::StructReturn& port
      );

      // ----------------------------------------------------------------------
      // Check serial output ports
      // ----------------------------------------------------------------------

      void check_serial(
          FppTest::Types::NoParams& port
      );

      void check_serial(
          FppTest::Types::PrimitiveParams& port
      );

      void check_serial(
          FppTest::Types::PortStringParams& port
      );

      void check_serial(
          FppTest::Types::EnumParams& port
      );

      void check_serial(
          FppTest::Types::ArrayParams& port
      );

      void check_serial(
          FppTest::Types::StructParams& port
      );

      void check_serial(
          FppTest::Types::SerialParam& port
      );

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
      Example component;

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

  };

#endif
