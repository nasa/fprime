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
      // send MS_DO_MATH command
      this->sendCmd_MS_DO_MATH(0,10,1.0,2.0,MathSenderComponentBase::ADD);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOut(0,1.0,2.0,MATH_ADD);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_MS_VAL1_SIZE(1);
      ASSERT_TLM_MS_VAL2_SIZE(1);
      ASSERT_TLM_MS_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_MS_VAL1(0,1.0);
      ASSERT_TLM_MS_VAL2(0,2.0);
      ASSERT_TLM_MS_OP(0,MathSenderComponentBase::ADD_TLM);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_MS_COMMAND_RECV(0,1.0,2.0,MathSenderComponentBase::ADD_EV);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0,MathSenderComponentBase::OPCODE_MS_DO_MATH,10,Fw::CmdResponse::OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathIn(0,10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_MS_RES_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_MS_RES(0,10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_RESULT_SIZE(1);
      // verify the expected value of the event arguments
      ASSERT_EVENTS_MS_RESULT(0,10.0);
  }

  void Tester ::
    testSubCommand()
  {
      // send MS_DO_MATH command
      this->sendCmd_MS_DO_MATH(0,10,1.0,2.0,MathSenderComponentBase::SUBTRACT);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOut(0,1.0,2.0,MATH_SUB);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_MS_VAL1_SIZE(1);
      ASSERT_TLM_MS_VAL2_SIZE(1);
      ASSERT_TLM_MS_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_MS_VAL1(0,1.0);
      ASSERT_TLM_MS_VAL2(0,2.0);
      ASSERT_TLM_MS_OP(0,MathSenderComponentBase::SUB_TLM);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_MS_COMMAND_RECV(0,1.0,2.0,MathSenderComponentBase::SUB_EV);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0,MathSenderComponentBase::OPCODE_MS_DO_MATH,10,Fw::CmdResponse::OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathIn(0,10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_MS_RES_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_MS_RES(0,10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_RESULT_SIZE(1);
      // verify the expect value of the event
      ASSERT_EVENTS_MS_RESULT(0,10.0);
  }

  void Tester ::
    testMultCommand()
  {
      // send MS_DO_MATH command
      this->sendCmd_MS_DO_MATH(0,10,1.0,2.0,MathSenderComponentBase::MULTIPLY);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOut(0,1.0,2.0,MATH_MULTIPLY);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_MS_VAL1_SIZE(1);
      ASSERT_TLM_MS_VAL2_SIZE(1);
      ASSERT_TLM_MS_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_MS_VAL1(0,1.0);
      ASSERT_TLM_MS_VAL2(0,2.0);
      ASSERT_TLM_MS_OP(0,MathSenderComponentBase::MULT_TLM);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_MS_COMMAND_RECV(0,1.0,2.0,MathSenderComponentBase::MULT_EV);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0,MathSenderComponentBase::OPCODE_MS_DO_MATH,10,Fw::CmdResponse::OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathIn(0,10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_MS_RES_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_MS_RES(0,10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_RESULT_SIZE(1);
      // verify the expect value of the event
      ASSERT_EVENTS_MS_RESULT(0,10.0);
  }

  void Tester ::
    testDivCommand()
  {
      // send MS_DO_MATH command
      this->sendCmd_MS_DO_MATH(0,10,1.0,2.0,MathSenderComponentBase::DIVIDE);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOut(0,1.0,2.0,MATH_DIVIDE);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_MS_VAL1_SIZE(1);
      ASSERT_TLM_MS_VAL2_SIZE(1);
      ASSERT_TLM_MS_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_MS_VAL1(0,1.0);
      ASSERT_TLM_MS_VAL2(0,2.0);
      ASSERT_TLM_MS_OP(0,MathSenderComponentBase::DIV_TLM);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_MS_COMMAND_RECV(0,1.0,2.0,MathSenderComponentBase::DIV_EV);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0,MathSenderComponentBase::OPCODE_MS_DO_MATH,10,Fw::CmdResponse::OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathIn(0,10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_MS_RES_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_MS_RES(0,10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_RESULT_SIZE(1);
      // verify the expect value of the event
      ASSERT_EVENTS_MS_RESULT(0,10.0);
  }
  void Tester ::
    toDo()
  {
    // TODO
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
    connectPorts()
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
    initComponents()
  {
    this->init();
    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );
  }

} // end namespace Ref
