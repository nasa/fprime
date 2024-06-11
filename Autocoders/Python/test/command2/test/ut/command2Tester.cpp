// ======================================================================
// \title  TestCommand.hpp
// \author tim
// \brief  cpp file for TestCommand test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "command2Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace AcTest {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  command2Tester ::
    command2Tester() :
      TestCommandGTestBase("Tester", MAX_HISTORY_SIZE),
      component("TestCommand")
  {
    this->initComponents();
    this->connectPorts();
  }

  command2Tester ::
    ~command2Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void command2Tester ::
    msgTest()
  {
      for (NATIVE_UINT_TYPE iter=0; iter <= QUEUE_DEPTH; iter++) {
        this->sendCmd_TEST_CMD_1(
                0, //!< The instance number
                10, //!< The command sequence number
                20, //!< The I32 command argument
                TestCommandComponentBase::MEMB1 //!< The ENUM argument
            );
      }
      ASSERT_EQ(this->component.m_msgsDropped,1);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void command2Tester ::
    connectPorts()
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

  void command2Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace AcTest
