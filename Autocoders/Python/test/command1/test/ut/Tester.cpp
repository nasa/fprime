// ======================================================================
// \title  Test1.hpp
// \author tim
// \brief  cpp file for Test1 test harness implementation class
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

namespace Cmd {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) :
#if FW_OBJECT_NAMES == 1
      Test1GTestBase("Tester", MAX_HISTORY_SIZE),
      component("Test1")
#else
      Test1GTestBase(MAX_HISTORY_SIZE),
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
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void)
  {

    // aport
    this->connect_to_aport(
        0,
        this->component.get_aport_InputPort(0)
    );

    // CmdDisp
    this->connect_to_CmdDisp(
        0,
        this->component.get_CmdDisp_InputPort(0)
    );

    // CmdStatus
    this->component.set_CmdStatus_OutputPort(
        0,
        this->get_from_CmdStatus(0)
    );

    // CmdReg
    this->component.set_CmdReg_OutputPort(
        0,
        this->get_from_CmdReg(0)
    );

  }

  void Tester ::
    initComponents(void)
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Cmd
