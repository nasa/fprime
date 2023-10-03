// ======================================================================
// \title  Example.hpp
// \author joshuaa
// \brief  cpp file for Example test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "noargportTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace ExampleComponents {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  noargportTester ::
    noargportTester() :
      ExampleGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Example")
  {
    this->initComponents();
    this->connectPorts();
  }

  noargportTester ::
    ~noargportTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void noargportTester ::
    testNoArgs()
  {
    this->invoke_to_noArgPort(0);
    this->invoke_to_asyncNoArgPort(0);
    this->component.doDispatch(); // Dispatch async port call
    this->invoke_to_guardedNoArgPort(0);
    ASSERT_EQ(this->invoke_to_exampleInput(0), 0);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void noargportTester ::
    connectPorts()
  {

    // noArgPort
    this->connect_to_noArgPort(
        0,
        this->component.get_noArgPort_InputPort(0)
    );

    // asyncNoArgPort
    this->connect_to_asyncNoArgPort(
        0,
        this->component.get_asyncNoArgPort_InputPort(0)
    );

    // guardedNoArgPort
    this->connect_to_guardedNoArgPort(
        0,
        this->component.get_guardedNoArgPort_InputPort(0)
    );



    // exampleInput
    this->connect_to_exampleInput(
        0,
        this->component.get_exampleInput_InputPort(0)
    );




  }

  void noargportTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH,
        INSTANCE
    );
  }

} // end namespace ExampleComponents
