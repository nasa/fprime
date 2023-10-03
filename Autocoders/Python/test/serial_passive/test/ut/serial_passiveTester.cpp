// ======================================================================
// \title  TestSerial.hpp
// \author tim
// \brief  cpp file for TestSerial test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "serial_passiveTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace TestComponents {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  serial_passiveTester ::
    serial_passiveTester() :
      TestSerialGTestBase("Tester", MAX_HISTORY_SIZE),
      component("TestSerial")
  {
    this->initComponents();
    this->connectPorts();
  }

  serial_passiveTester ::
    ~serial_passiveTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void serial_passiveTester ::
    toDo()
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Handlers for serial from ports
  // ----------------------------------------------------------------------

  void serial_passiveTester ::
    from_SerialOut_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void serial_passiveTester ::
    connectPorts()
  {

  }

  void serial_passiveTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace TestComponents
