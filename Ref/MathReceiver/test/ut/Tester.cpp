// ======================================================================
// \title  MathReceiver.hpp
// \author tcanham, bocchino
// \brief  cpp file for MathReceiver test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"

#define INSTANCE 0
#define CMD_SEQ 10
#define MAX_HISTORY_SIZE 10
#define QUEUE_DEPTH 10

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      MathReceiverGTestBase("Tester", MAX_HISTORY_SIZE),
      component("MathReceiver")
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
      // Set the factor parameter by command
      const F32 factor = 3.0;
      this->setFactor(factor);
      // Do the add operation
      this->doMathOp(2.0, MathOp::ADD, 3.0, factor);
  }

  void Tester ::
    testSubCommand()
  {
      // Set the factor parameter by loading parameters
      const F32 factor = 3.0;
      this->paramSet_FACTOR(factor, Fw::ParamValid::VALID);
      this->component.loadParameters();
      // Do the operation
      this->doMathOp(2.0, MathOp::SUB, 3.0, factor);
  }

  void Tester ::
    testMulCommand()
  {
      // Set the factor parameter by command
      const F32 factor = 3.0;
      this->setFactor(factor);
      // Do the add operation
      this->doMathOp(2.0, MathOp::MUL, 3.0, factor);
  }

  void Tester ::
    testDivCommand()
  {
      // Set the factor parameter by loading parameters
      const F32 factor = 3.0;
      this->paramSet_FACTOR(factor, Fw::ParamValid::VALID);
      this->component.loadParameters();
      // Do the operation
      this->doMathOp(2.0, MathOp::DIV, 3.0, factor);
  }

  void Tester ::
    testThrottle()
  {
      // send the number of commands required to throttle the event
      // Use the autocoded value so the unit test passes if the
      // throttle value is changed
      for (
          U16 cycle = 0;
          cycle < MathReceiverComponentBase::EVENTID_FACTOR_UPDATED_THROTTLE;
          cycle++
      ) {
          this->setFactor(1.0);
      }

      // Event should now be throttled
      this->setFactor(1.0, ThrottleState::THROTTLED);

      // send the command to clear the throttle
      this->sendCmd_CLEAR_EVENT_THROTTLE(INSTANCE, CMD_SEQ);
      // invoke scheduler port to dispatch message
      this->invoke_to_schedIn(0, 0);
      // verify clear event was sent
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_THROTTLE_CLEARED_SIZE(1);

      // Throttling should be cleared
      this->setFactor(1.0);

  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_mathResultOut_handler(
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
    this->pushFromPortEntry_mathResultOut(result);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    setFactor(
        F32 factor,
        ThrottleState throttleState
    )
  {
      // clear history
      this->clearHistory();
      // set the parameter
      this->paramSet_FACTOR(factor, Fw::ParamValid::VALID);
      this->paramSend_FACTOR(INSTANCE, CMD_SEQ);
      if (throttleState == ThrottleState::NOT_THROTTLED) {
          // verify the parameter update notification event was sent
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_FACTOR_UPDATED_SIZE(1);
          ASSERT_EVENTS_FACTOR_UPDATED(0, factor);
      }
      else {
          ASSERT_EVENTS_SIZE(0);
      }
  }

  F32 Tester ::
    computeResult(
        F32 val1,
        MathOp op,
        F32 val2,
        F32 factor
    )
  {
      F32 result = 0;
      switch (op.e) {
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

  void Tester ::
    doMathOp(
        F32 val1,
        MathOp op,
        F32 val2,
        F32 factor
    )
  {

      // clear history
      this->clearHistory();

      // invoke operation port with add operation
      this->invoke_to_mathOpIn(0, val1, op, val2);
      // invoke scheduler port to dispatch message
      this->invoke_to_schedIn(0, 0);

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
      ASSERT_EVENTS_SIZE(1);
      // check that it was the op event
      ASSERT_EVENTS_OPERATION_PERFORMED_SIZE(1);
      // check that the event has the correct argument
      ASSERT_EVENTS_OPERATION_PERFORMED(0, op);

      // verify telemetry

      // check that one channel was written
      ASSERT_TLM_SIZE(1);
      // check that it was the op channel
      ASSERT_TLM_OPERATION_SIZE(1);
      // check for the correct value of the channel
      ASSERT_TLM_OPERATION(0, op);

  }

  void Tester ::
    connectPorts()
  {

    // mathOpIn
    this->connect_to_mathOpIn(
        0,
        this->component.get_mathOpIn_InputPort(0)
    );

    // schedIn
    this->connect_to_schedIn(
        0,
        this->component.get_schedIn_InputPort(0)
    );

    // cmdIn
    this->connect_to_cmdIn(
        0,
        this->component.get_cmdIn_InputPort(0)
    );

    // mathResultOut
    this->component.set_mathResultOut_OutputPort(
        0,
        this->get_from_mathResultOut(0)
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

    // prmGetOut
    this->component.set_prmGetOut_OutputPort(
        0,
        this->get_from_prmGetOut(0)
    );

    // prmSetOut
    this->component.set_prmSetOut_OutputPort(
        0,
        this->get_from_prmSetOut(0)
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
    this->component.init(QUEUE_DEPTH, INSTANCE);
  }

} // end namespace Ref
