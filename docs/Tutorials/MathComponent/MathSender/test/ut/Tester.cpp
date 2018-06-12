// ====================================================================== 
// \title  MathSender.hpp
// \author tcanham
// \brief  cpp file for MathSender test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) : 
#if FW_OBJECT_NAMES == 1
      MathSenderGTestBase("Tester", MAX_HISTORY_SIZE),
      component("MathSender")
#else
      MathSenderGTestBase(MAX_HISTORY_SIZE),
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
    testOperationCommand(void)
  {
      // send MS_DO_MATH command
      this->sendCmd_MS_DO_MATH(0,10,1.0,2.0,MathSenderComponentBase::ADD);
      // retrieve the message from the message queue and dispatch
      this->component.doDispatch();
      // verify that the output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      ASSERT_from_mathOut_SIZE(1);
      ASSERT_from_mathOut(0,1.0,2.0,MATH_ADD);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      ASSERT_TLM_MS_VAL1_SIZE(1);
      ASSERT_TLM_MS_VAL1(0,1.0);
      ASSERT_TLM_MS_VAL2_SIZE(1);
      ASSERT_TLM_MS_VAL2(0,2.0);
      ASSERT_TLM_MS_OP_SIZE(1);
      ASSERT_TLM_MS_OP(0,MathSenderComponentBase::ADD_TLM);
      // verify events were sent
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_MS_COMMAND_RECV(0,1.0,2.0,MathSenderComponentBase::ADD_EV);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,MathSenderComponentBase::OPCODE_MS_DO_MATH,10,Fw::COMMAND_OK);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_mathOut_handler(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        F32 val2,
        MathOperation operation
    )
  {
    this->pushFromPortEntry_mathOut(val1, val2, operation);
  }

  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void) 
  {

    // mathIn
    this->connect_to_mathIn(
        0,
        this->component.get_mathIn_InputPort(0)
    );

    // CmdDisp
    this->connect_to_CmdDisp(
        0,
        this->component.get_CmdDisp_InputPort(0)
    );

    // mathOut
    this->component.set_mathOut_OutputPort(
        0, 
        this->get_from_mathOut(0)
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

    // Tlm
    this->component.set_Tlm_OutputPort(
        0, 
        this->get_from_Tlm(0)
    );

    // Time
    this->component.set_Time_OutputPort(
        0, 
        this->get_from_Time(0)
    );

    // Log
    this->component.set_Log_OutputPort(
        0, 
        this->get_from_Log(0)
    );

    // LogText
    this->component.set_LogText_OutputPort(
        0, 
        this->get_from_LogText(0)
    );

  }

  void Tester ::
    initComponents(void) 
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace Ref
