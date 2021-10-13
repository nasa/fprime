// ======================================================================
// \title  MathSender.hpp
// \author tcanham
// \brief  cpp file for MathSender test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
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
    Tester() :
      MathSenderGTestBase("Tester", MAX_HISTORY_SIZE),
      component("MathSender")
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    testAddCommand()
  {
      MathOp add = MathOp::ADD;
      // send MS_DO_MATH command
      this->sendCmd_DO_MATH(0, 10, 1.0, add, 2.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOpOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOpOut(0, 1.0, add, 2.0);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_VAL1_SIZE(1);
      ASSERT_TLM_VAL2_SIZE(1);
      ASSERT_TLM_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_VAL1(0, 1.0);
      ASSERT_TLM_VAL2(0, 2.0);
      ASSERT_TLM_OP(0, add);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_COMMAND_RECV(0, 1.0, add, 2.0);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0, MathSenderComponentBase::OPCODE_DO_MATH, 10, Fw::CmdResponse::OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathResultIn(0, 10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_RESULT_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_RESULT(0, 10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_RESULT_SIZE(1);
      // verify the expected value of the event arguments
      ASSERT_EVENTS_RESULT(0, 10.0);
  }

  void Tester ::
    testSubCommand()
  {
      MathOp sub = MathOp::SUB;
      // send MS_DO_MATH command
      this->sendCmd_DO_MATH(0, 10, 1.0, sub, 2.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOpOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOpOut(0, 1.0, sub, 2.0);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_VAL1_SIZE(1);
      ASSERT_TLM_VAL2_SIZE(1);
      ASSERT_TLM_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_VAL1(0, 1.0);
      ASSERT_TLM_VAL2(0, 2.0);
      ASSERT_TLM_OP(0, sub);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_COMMAND_RECV(0, 1.0, sub, 2.0);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0, MathSenderComponentBase::OPCODE_DO_MATH, 10, Fw::CmdResponse::OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathResultIn(0, 10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_RESULT_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_RESULT(0, 10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_RESULT_SIZE(1);
      // verify the expect value of the event
      ASSERT_EVENTS_RESULT(0, 10.0);
  }

  void Tester ::
    testMultCommand()
  {
      MathOp mul = MathOp::MUL;
      // send MS_DO_MATH command
      this->sendCmd_DO_MATH(0, 10, 1.0, mul, 2.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOpOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOpOut(0, 1.0, mul, 2.0);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_VAL1_SIZE(1);
      ASSERT_TLM_VAL2_SIZE(1);
      ASSERT_TLM_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_VAL1(0, 1.0);
      ASSERT_TLM_VAL2(0, 2.0);
      ASSERT_TLM_OP(0, mul);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_COMMAND_RECV(0, 1.0, MathOp::MUL, 2.0);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0, MathSenderComponentBase::OPCODE_DO_MATH, 10, Fw::CmdResponse::OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathResultIn(0, 10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_RESULT_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_RESULT(0, 10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_RESULT_SIZE(1);
      // verify the expect value of the event
      ASSERT_EVENTS_RESULT(0, 10.0);
  }

  void Tester ::
    testDivCommand()
  {
      MathOp div = MathOp::DIV;
      // send MS_DO_MATH command
      this->sendCmd_DO_MATH(0, 10, 1.0, div, 2.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOpOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOpOut(0, 1.0, div, 2.0);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_VAL1_SIZE(1);
      ASSERT_TLM_VAL2_SIZE(1);
      ASSERT_TLM_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_VAL1(0, 1.0);
      ASSERT_TLM_VAL2(0, 2.0);
      ASSERT_TLM_OP(0, div);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_COMMAND_RECV(0, 1.0, MathOp::DIV, 2.0);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0, MathSenderComponentBase::OPCODE_DO_MATH, 10, Fw::CmdResponse::OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathResultIn(0, 10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_RESULT_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_RESULT(0, 10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_RESULT_SIZE(1);
      // verify the expect value of the event
      ASSERT_EVENTS_RESULT(0, 10.0);
  }
  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_mathOpOut_handler(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        const MathOp& op,
        F32 val2
    )
  {
    this->pushFromPortEntry_mathOpOut(val1, op, val2);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // mathResultIn
    this->connect_to_mathResultIn(
        0,
        this->component.get_mathResultIn_InputPort(0)
    );

    // cmdIn
    this->connect_to_cmdIn(
        0,
        this->component.get_cmdIn_InputPort(0)
    );

    // mathOpOut
    this->component.set_mathOpOut_OutputPort(
        0,
        this->get_from_mathOpOut(0)
    );

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(
        0,
        this->get_from_cmdResponseOut(0)
    );

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(
        0,
        this->get_from_cmdRegOut(0)
    );

    // tlmOut
    this->component.set_tlmOut_OutputPort(
        0,
        this->get_from_tlmOut(0)
    );

    // timeGetOut
    this->component.set_timeGetOut_OutputPort(
        0,
        this->get_from_timeGetOut(0)
    );

    // eventOut
    this->component.set_eventOut_OutputPort(
        0,
        this->get_from_eventOut(0)
    );

    // textEventOut
    this->component.set_textEventOut_OutputPort(
        0,
        this->get_from_textEventOut(0)
    );

  }

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace Ref
