// ======================================================================
// \title  Test1.hpp
// \author tcanham
// \brief  cpp file for Test1 test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "command_resTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Cmd {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  command_resTester ::
    command_resTester() :
      Test1GTestBase("Tester", MAX_HISTORY_SIZE),
      component("Test1")
  {
    this->initComponents();
    this->connectPorts();
  }

  command_resTester ::
    ~command_resTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void command_resTester ::
      residualTest()
  {
      // This test will do different things based on the configuration macro FW_CMD_CHECK_RESIDUAL
      // If it is on, a command failure will be checked. If not, command success will be checked.

      // Manually compose a buffer that is too large.
      // test command has 9 bytes of argument:
      // I32, F32, I8
      // Serialize more than that.
      Fw::CmdArgBuffer args;
      I32 arg1 = 20;
      F32 arg2 = 20.1;
      I8 arg3 = 49;
      U32 arg4 = 1000;

      ASSERT_EQ(Fw::FW_SERIALIZE_OK,args.serialize(arg1));
      ASSERT_EQ(Fw::FW_SERIALIZE_OK,args.serialize(arg2));
      ASSERT_EQ(Fw::FW_SERIALIZE_OK,args.serialize(arg3));
      ASSERT_EQ(Fw::FW_SERIALIZE_OK,args.serialize(arg4));

      this->sendRawCmd(Test1ComponentBase::OPCODE_TEST_CMD_1,10,args);

      // check reply
#if FW_CMD_CHECK_RESIDUAL
      // should fail
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,Test1ComponentBase::OPCODE_TEST_CMD_1,10,Fw::CmdResponse::FORMAT_ERROR);
#else
      // should pass
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,Test1ComponentBase::OPCODE_TEST_CMD_1,10,Fw::CmdResponse::OK);
#endif

      // second async command
      args.resetSer();
      U32 arg21 = 0;
      U16 arg22 = 0;
      U8 arg23 = 0;

      ASSERT_EQ(Fw::FW_SERIALIZE_OK,args.serialize(arg21));
      ASSERT_EQ(Fw::FW_SERIALIZE_OK,args.serialize(arg22));
      ASSERT_EQ(Fw::FW_SERIALIZE_OK,args.serialize(arg23));
      ASSERT_EQ(Fw::FW_SERIALIZE_OK,args.serialize(arg4));

      this->sendRawCmd(Test1ComponentBase::OPCODE_TEST_CMD_2,20,args);

      this->clearHistory();
      // dispatch
      ASSERT_EQ(Fw::QueuedComponentBase::MSG_DISPATCH_OK,this->component.doDispatch());

      // check reply
#if FW_CMD_CHECK_RESIDUAL
      // should fail
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,Test1ComponentBase::OPCODE_TEST_CMD_2,20,Fw::CmdResponse::FORMAT_ERROR);
#else
      // should pass
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,Test1ComponentBase::OPCODE_TEST_CMD_2,20,Fw::CmdResponse::OK);
#endif

  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void command_resTester ::
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

  void command_resTester ::
    initComponents()
  {
    this->init();
    this->component.init(QUEUE_DEPTH,
        INSTANCE
    );
  }

} // end namespace Cmd
