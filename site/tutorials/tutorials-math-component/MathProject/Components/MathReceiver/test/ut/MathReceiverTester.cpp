// ======================================================================
// \title  MathReceiver.hpp
// \author asloan
// \brief  cpp file for MathReceiver test harness implementation class
// ======================================================================

#include "MathReceiverTester.hpp"
#include "STest/Pick/Pick.hpp"

namespace MathProject
{
#define CMD_SEQ 42
  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  MathReceiverTester ::
      MathReceiverTester() : MathReceiverGTestBase("Tester", MathReceiverTester::MAX_HISTORY_SIZE),
                             component("MathReceiver")
  {
    this->initComponents();
    this->connectPorts();
  }

  MathReceiverTester ::
      ~MathReceiverTester()
  {
      this->component.deinit();
  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void MathReceiverTester ::
      toDo()
  {
    // TODO
  }

  F32 MathReceiverTester ::
      pickF32Value()
  {
    const F32 m = 10e6;
    return m * (1.0 - 2 * STest::Pick::inUnitInterval());
  }

  void MathReceiverTester ::
      setFactor(
          F32 factor,
          ThrottleState throttleState)
  {
    // clear history
    this->clearHistory();
    // set the parameter
    this->paramSet_FACTOR(factor, Fw::ParamValid::VALID);
    const U32 instance = STest::Pick::any();
    const U32 cmdSeq = STest::Pick::any();
    this->paramSend_FACTOR(instance, cmdSeq);
    if (throttleState == ThrottleState::NOT_THROTTLED)
    {
      // verify the parameter update notification event was sent
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_FACTOR_UPDATED_SIZE(1);
      ASSERT_EVENTS_FACTOR_UPDATED(0, factor);
    }
    else
    {
      ASSERT_EVENTS_SIZE(0);
    }
  }

  F32 MathReceiverTester ::
      computeResult(
          F32 val1,
          MathOp op,
          F32 val2,
          F32 factor)
  {
    F32 result = 0;
    switch (op.e)
    {
    case MathOp::ADD:
      result = val1 + val2;
      break;
    case MathOp::SUB:
      result = val1 - val2;
      break;
    case MathOp::MUL:
      result = val1 * val2;
      break;
    case MathOp::DIV:
      result = val1 / val2;
      break;
    default:
      FW_ASSERT(0, op.e);
      break;
    }
    result *= factor;
    return result;
  }

  void MathReceiverTester ::
      doMathOp(
          MathOp op,
          F32 factor)
  {

    // pick values
    const F32 val1 = pickF32Value();
    const F32 val2 = pickF32Value();

    // clear history
    this->clearHistory();

    // invoke operation port with add operation
    this->invoke_to_mathOpIn(0, val1, op, val2);
    // invoke scheduler port to dispatch message
    const U32 context = STest::Pick::any();
    this->invoke_to_schedIn(0, context);

    // verify the result of the operation was returned

    // check that there was one port invocation
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    // check that the port we expected was invoked
    ASSERT_from_mathResultOut_SIZE(1);
    // check that the component performed the operation correctly
    const F32 result = computeResult(val1, op, val2, factor);
    ASSERT_from_mathResultOut(0, result);

    // verify events

    // check that there was one event
    // if you're dviding by zero, there may be two events ;)
    ASSERT_EVENTS_SIZE(1);
    // check that it was the op event
    ASSERT_EVENTS_OPERATION_PERFORMED_SIZE(1);
    // check that the event has the correct argument
    ASSERT_EVENTS_OPERATION_PERFORMED(0, op);

    // verify telemetry

    // check that one channel was written
    ASSERT_TLM_SIZE(2);
    // check that it was the op channel
    ASSERT_TLM_OPERATION_SIZE(1);
    // check for the correct value of the channel
    ASSERT_TLM_OPERATION(0, op);
  }

  void MathReceiverTester ::
      testAdd()
  {
    // Set the factor parameter by command
    const F32 factor = pickF32Value();
    this->setFactor(factor, ThrottleState::NOT_THROTTLED);
    // Do the add operation
    this->doMathOp(MathOp::ADD, factor);
  }

  void MathReceiverTester ::
      testSub()
  {
    // Set the factor parameter by loading parameters
    const F32 factor = pickF32Value();
    this->paramSet_FACTOR(factor, Fw::ParamValid::VALID);
    this->component.loadParameters();
    // Do the operation
    this->doMathOp(MathOp::SUB, factor);
  }

  void MathReceiverTester ::
      testThrottle()
  {

    // send the number of commands required to throttle the event
    // Use the autocoded value so the unit test passes if the
    // throttle value is changed
    const F32 factor = pickF32Value();
    for (
        U16 cycle = 0;
        cycle < MathReceiverComponentBase::EVENTID_FACTOR_UPDATED_THROTTLE;
        cycle++)
    {
      this->setFactor(factor, ThrottleState::NOT_THROTTLED);
    }

    // Event should now be throttled
    this->setFactor(factor, ThrottleState::THROTTLED);

    // send the command to clear the throttle
    this->sendCmd_CLEAR_EVENT_THROTTLE(TEST_INSTANCE_ID, CMD_SEQ);
    // invoke scheduler port to dispatch message
    const U32 context = STest::Pick::any();
    this->invoke_to_schedIn(0, context);
    // verify clear event was sent
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_THROTTLE_CLEARED_SIZE(1);

    // Throttling should be cleared
    this->setFactor(factor, ThrottleState::NOT_THROTTLED);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void MathReceiverTester ::
      from_mathResultOut_handler(
          const FwIndexType portNum,
          F32 result)
  {
    this->pushFromPortEntry_mathResultOut(result);
  }

} // end namespace MathProject
