// ======================================================================
// \title  Example/test/ut/Tester.hpp
// \author tchieu
// \brief  hpp file for Example test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "FppTest/port/Example.hpp"
#include "FppTest/port/PortTypes.hpp"

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
          FppTest::Port::NoArgsPort& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Port::PrimitiveArgsPort& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Port::NoArgsReturnPort& port
      );

      void invoke(
          NATIVE_INT_TYPE portNum,
          FppTest::Port::PrimitiveReturnPort& port
      );

      // ----------------------------------------------------------------------
      // Check history of typed output ports
      // ----------------------------------------------------------------------

      void check_history(
          NATIVE_INT_TYPE portNum,
          FppTest::Port::NoArgsPort& port
      );

      void check_history(
          NATIVE_INT_TYPE portNum,
          FppTest::Port::PrimitiveArgsPort& port
      );

      void check_history(
          NATIVE_INT_TYPE portNum,
          FppTest::Port::NoArgsReturnPort& port
      );
      
      void check_history(
          NATIVE_INT_TYPE portNum,
          FppTest::Port::PrimitiveReturnPort& port
      );

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_arrayArgsOut
      //!
      void from_arrayArgsOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortArray &a, /*!< 
      An array
      */
          PortArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler for from_arrayReturnOut
      //!
      PortArray from_arrayReturnOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortArray &a, /*!< 
      An array
      */
          PortArray &aRef /*!< 
      An array ref
      */
      );

      //! Handler for from_enumArgsOut
      //!
      void from_enumArgsOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortEnum &e, /*!< 
      An enum
      */
          PortEnum &eRef /*!< 
      An enum ref
      */
      );

      //! Handler for from_enumReturnOut
      //!
      PortEnum from_enumReturnOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortEnum &e, /*!< 
      An enum
      */
          PortEnum &eRef /*!< 
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
          const PortStruct &s, /*!< 
      A struct
      */
          PortStruct &sRef /*!< 
      A struct ref
      */
      );

      //! Handler for from_structReturnOut
      //!
      PortStruct from_structReturnOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const PortStruct &s, /*!< 
      A struct
      */
          PortStruct &sRef /*!< 
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

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      Example component;

      // Values returned by typed output ports
      FppTest::Port::BoolReturn noArgsReturnVal;
      FppTest::Port::PrimitiveReturn primitiveReturnVal;

  };

#endif
