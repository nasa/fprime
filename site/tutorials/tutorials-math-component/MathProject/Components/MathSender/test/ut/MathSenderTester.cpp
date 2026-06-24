// ======================================================================
// \title  MathSender.hpp
// \author asloan
// \brief  cpp file for MathSender test harness implementation class
// ======================================================================

#include "MathSenderTester.hpp"
#include "STest/Pick/Pick.hpp"

namespace MathProject
{

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  MathSenderTester ::
      MathSenderTester() : MathSenderGTestBase("Tester", MathSenderTester::MAX_HISTORY_SIZE),
                           component("MathSender")
  {
    this->initComponents();
    this->connectPorts();
  }

  MathSenderTester ::
      ~MathSenderTester()
  {
      this->component.deinit();
  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void MathSenderTester ::
      toDo()
  {
    // TODO
  }

  void MathSenderTester ::
      testDoMath(MathOp op)
  {
    // Pick values
    const F32 val1 = 2.0;
    const F32 val2 = 3.0;
    // Send the command
    // pick a command sequence number
    const U32 cmdSeq = 10;
    // send DO_MATH command
    this->sendCmd_DO_MATH(0, cmdSeq, val1, op, val2);
    // retrieve the message from the message queue and dispatch the command to the handler
    this->component.doDispatch();
    // Verify command receipt and response
    // verify command response was sent
    ASSERT_CMD_RESPONSE_SIZE(1);
    // verify the command response was correct as expected
    ASSERT_CMD_RESPONSE(0, MathSenderComponentBase::OPCODE_DO_MATH, cmdSeq, Fw::CmdResponse::OK);
    // Verify operation request on mathOpOut
    // verify that one output port was invoked overall
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    // verify that the math operation port was invoked once
    ASSERT_from_mathOpOut_SIZE(1);
    // verify the arguments of the operation port
    ASSERT_from_mathOpOut(0, val1, op, val2);
    // Verify telemetry
    // verify that 3 channels were written
    ASSERT_TLM_SIZE(3);
    // verify that the desired telemetry values were sent once
    ASSERT_TLM_VAL1_SIZE(1);
    ASSERT_TLM_VAL2_SIZE(1);
    ASSERT_TLM_OP_SIZE(1);
    // verify that the correct telemetry values were sent
    ASSERT_TLM_VAL1(0, val1);
    ASSERT_TLM_VAL2(0, val2);
    ASSERT_TLM_OP(0, op);
    // Verify event reports
    // verify that one event was sent
    ASSERT_EVENTS_SIZE(1);
    // verify the expected event was sent once
    ASSERT_EVENTS_COMMAND_RECV_SIZE(1);
    // verify the correct event arguments were sent
    ASSERT_EVENTS_COMMAND_RECV(0, val1, op, val2);
  }

  void MathSenderTester ::
      testAddCommand()
  {
    this->testDoMath(MathOp::ADD);
  }

  void MathSenderTester ::
      testResult()
  {
    // Generate an expected result
    const F32 result = 10.0;
    // reset all telemetry and port history
    this->clearHistory();
    // call result port with result
    this->invoke_to_mathResultIn(0, result);
    // retrieve the message from the message queue and dispatch the command to the handler
    this->component.doDispatch();
    // verify one telemetry value was written
    ASSERT_TLM_SIZE(1);
    // verify the desired telemetry channel was sent once
    ASSERT_TLM_RESULT_SIZE(1);
    // verify the values of the telemetry channel
    ASSERT_TLM_RESULT(0, result);
    // verify one event was sent
    ASSERT_EVENTS_SIZE(1);
    // verify the expected event was sent once
    ASSERT_EVENTS_RESULT_SIZE(1);
    // verify the expect value of the event
    ASSERT_EVENTS_RESULT(0, result);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void MathSenderTester ::
      from_mathOpOut_handler(
          const FwIndexType portNum,
          F32 val1,
          const MathProject::MathOp &op,
          F32 val2)
  {
    this->pushFromPortEntry_mathOpOut(val1, op, val2);
  }

} // end namespace MathProject
