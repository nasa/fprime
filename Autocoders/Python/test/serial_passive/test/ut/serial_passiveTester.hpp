// ======================================================================
// \title  TestSerial/test/ut/Tester.hpp
// \author tim
// \brief  hpp file for TestSerial test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "serial_passiveGTestBase.hpp"
#include "Autocoders/Python/test/serial_passive/TestSerialImpl.hpp"

namespace TestComponents {

  class serial_passiveTester :
    public TestSerialGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object serial_passiveTester
      //!
      serial_passiveTester();

      //! Destroy object serial_passiveTester
      //!
      ~serial_passiveTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!
      void toDo();

    private:

      // ----------------------------------------------------------------------
      // Handlers for serial from ports
      // ----------------------------------------------------------------------

      //! Handler for from_SerialOut
      //!
      void from_SerialOut_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
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
      TestSerialImpl component;

  };

} // end namespace TestComponents

#endif
