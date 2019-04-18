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

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace TestComponents {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) :
#if FW_OBJECT_NAMES == 1
      TestSerialGTestBase("Tester", MAX_HISTORY_SIZE),
      component("TestSerial")
#else
      TestSerialGTestBase(MAX_HISTORY_SIZE),
      component()
#endif
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester(void)
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    toDo(void)
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Handlers for serial from ports
  // ----------------------------------------------------------------------

  void Tester ::
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

  void Tester ::
    connectPorts(void)
  {

  }

  void Tester ::
    initComponents(void)
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace TestComponents
