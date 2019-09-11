// ====================================================================== 
// \title  MathReceiver.hpp
// \author tcanham
// \brief  cpp file for MathReceiver test harness implementation class
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
#define QUEUE_DEPTH 10

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) : 
#if FW_OBJECT_NAMES == 1
      MathReceiverGTestBase("Tester", MAX_HISTORY_SIZE),
      component("MathReceiver")
#else
      MathReceiverGTestBase(MAX_HISTORY_SIZE),
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
    testAddCommand(void) 
  {
      // load parameters
      this->component.loadParameters();
      // invoke operation port with add operation
      this->invoke_to_mathIn(0,2.0,3.0,Ref::MATH_ADD);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);

      // verify the result of the operation was returned

      // check that there was one and only one port invocation
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // check that only the port we expected was invoked
      ASSERT_from_mathOut_SIZE(1);
      // check that the component did the operation correctly. 
      // Since factor1 is the default value of 0, result will be zero
      ASSERT_from_mathOut(0,0.0);
      // verify telemetry and events

      // the event and telemetry channel use the Ref::MathOp type for values
      Ref::MathOp checkOp(2.0,3.0,Ref::ADD,0.0);

      // check that there was only one event
      ASSERT_EVENTS_SIZE(1);
      // check that it was the op event
      ASSERT_EVENTS_MR_OPERATION_PERFORMED_SIZE(1);
      // check that the event has the correct argument
      ASSERT_EVENTS_MR_OPERATION_PERFORMED(0,checkOp);

      // check that there was only one channel written
      ASSERT_TLM_SIZE(1);
      // check that it was the op channel
      ASSERT_TLM_MR_OPERATION_SIZE(1);
      // check for the correct value of the channel
      ASSERT_TLM_MR_OPERATION(0,checkOp);

      // clear the history before sending the command
      this->clearHistory();
      // send the command to set factor1 to 2.0
      this->sendCmd_MR_SET_FACTOR1(0,10,2.0);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);
      // verify the changed value events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1(0,2.0);
      // verify the changed value channel
      ASSERT_TLM_SIZE(2);
      ASSERT_TLM_MR_FACTOR1_SIZE(1);
      ASSERT_TLM_MR_FACTOR1(0,2.0);
      ASSERT_TLM_MR_FACTOR1S_SIZE(1);
      ASSERT_TLM_MR_FACTOR1S(0,1);
      // verify the command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,MathReceiverComponentBase::OPCODE_MR_SET_FACTOR1,10,Fw::COMMAND_OK);

      // Repeat the operation with the new factor

      // clear the history before sending the port call
      this->clearHistory();

      // invoke operation port with add operation
      this->invoke_to_mathIn(0,2.0,3.0,Ref::MATH_ADD);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);

      // verify the result of the operation was returned

      // check that there was one and only one port invocation
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // check that only the port we expected was invoked
      ASSERT_from_mathOut_SIZE(1);
      // check that the component did the operation correctly. 
      // Now that factor1 is updated, the result should be:
      F32 result = (2.0+3.0)*2.0;
      ASSERT_from_mathOut(0,result);
      // verify telemetry and events

      // the event and telemetry channel use the Ref::MathOp type for values
      checkOp.set(2.0,3.0,Ref::ADD,result);

      // check that there was only one event
      ASSERT_EVENTS_SIZE(1);
      // check that it was the op event
      ASSERT_EVENTS_MR_OPERATION_PERFORMED_SIZE(1);
      // check that the event has the correct argument
      ASSERT_EVENTS_MR_OPERATION_PERFORMED(0,checkOp);

      // check that there was only one channel written
      ASSERT_TLM_SIZE(1);
      // check that it was the op channel
      ASSERT_TLM_MR_OPERATION_SIZE(1);
      // check for the correct value of the channel
      ASSERT_TLM_MR_OPERATION(0,checkOp);

      // clear the history
      this->clearHistory();

      // set the test value for the parameter
      this->paramSet_factor2(3.0,Fw::PARAM_VALID);
      // now send the factor2 parameter to the component
      this->paramSend_factor2(0,0);
      // verify the parameter update notification event was sent
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_MR_UPDATED_FACTOR2_SIZE(1);
      ASSERT_EVENTS_MR_UPDATED_FACTOR2(0,3.0);

      // Do the computation again and verify that the parameter was used

      // clear the history
      this->clearHistory();

      // invoke operation port with add operation
      this->invoke_to_mathIn(0,2.0,3.0,Ref::MATH_ADD);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);

      // verify the result of the operation was returned

      // check that there was one and only one port invocation
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // check that only the port we expected was invoked
      ASSERT_from_mathOut_SIZE(1);
      // check that the component did the operation correctly. 
      // Now that factor2 parameter is updated, the result should be:
      result = (2.0+3.0)*2.0/3.0;
      ASSERT_from_mathOut(0,result);
      // verify telemetry and events

      // the event and telemetry channel use the Ref::MathOp type for values
      checkOp.set(2.0,3.0,Ref::ADD,result);

      // check that there was only one event
      ASSERT_EVENTS_SIZE(1);
      // check that it was the op event
      ASSERT_EVENTS_MR_OPERATION_PERFORMED_SIZE(1);
      // check that the event has the correct argument
      ASSERT_EVENTS_MR_OPERATION_PERFORMED(0,checkOp);

      // check that there was only one channel written
      ASSERT_TLM_SIZE(1);
      // check that it was the op channel
      ASSERT_TLM_MR_OPERATION_SIZE(1);
      // check for the correct value of the channel
      ASSERT_TLM_MR_OPERATION(0,checkOp);
      
}

  void Tester ::
    testSubCommand(void) 
  {
      // set the test value for the parameter before loading - it will be initialized to this value
      this->paramSet_factor2(5.0,Fw::PARAM_VALID);

      // load parameters
      this->component.loadParameters();

      // send the command to set factor1 to 2.0
      this->sendCmd_MR_SET_FACTOR1(0,10,2.0);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);
      // verify the changed value events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1(0,2.0);
      // verify the changed value channel
      ASSERT_TLM_SIZE(2);
      ASSERT_TLM_MR_FACTOR1_SIZE(1);
      ASSERT_TLM_MR_FACTOR1(0,2.0);
      ASSERT_TLM_MR_FACTOR1S_SIZE(1);
      ASSERT_TLM_MR_FACTOR1S(0,1);
      // verify the command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,MathReceiverComponentBase::OPCODE_MR_SET_FACTOR1,10,Fw::COMMAND_OK);
      
      // clear the history
      this->clearHistory();

      // invoke operation port with add operation
      this->invoke_to_mathIn(0,2.0,3.0,Ref::MATH_SUB);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);

      // verify the result of the operation was returned
      F32 result = (2.0-3.0)*2.0/5.0;
      // the event and telemetry channel use the Ref::MathOp type for values
      Ref::MathOp checkOp(2.0,3.0,Ref::SUB,result);

      // check that there was one and only one port invocation
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // check that only the port we expected was invoked
      ASSERT_from_mathOut_SIZE(1);
      // check that the component did the operation correctly. 
      ASSERT_from_mathOut(0,result);
      // verify telemetry and events

      // check that there was only one event
      ASSERT_EVENTS_SIZE(1);
      // check that it was the op event
      ASSERT_EVENTS_MR_OPERATION_PERFORMED_SIZE(1);
      // check that the event has the correct argument
      ASSERT_EVENTS_MR_OPERATION_PERFORMED(0,checkOp);

      // check that there was only one channel written
      ASSERT_TLM_SIZE(1);
      // check that it was the op channel
      ASSERT_TLM_MR_OPERATION_SIZE(1);
      // check for the correct value of the channel
      ASSERT_TLM_MR_OPERATION(0,checkOp);
  }

  void Tester ::
    testMultCommand(void) 
  {
      // set the test value for the parameter before loading - it will be initialized to this value
      this->paramSet_factor2(-1.0,Fw::PARAM_VALID);

      // load parameters
      this->component.loadParameters();

      // send the command to set factor1 to 2.0
      this->sendCmd_MR_SET_FACTOR1(0,10,2.0);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);
      // verify the changed value events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1(0,2.0);
      ASSERT_TLM_MR_FACTOR1S_SIZE(1);
      ASSERT_TLM_MR_FACTOR1S(0,1);
      // verify the changed value channel
      ASSERT_TLM_SIZE(2);
      ASSERT_TLM_MR_FACTOR1_SIZE(1);
      ASSERT_TLM_MR_FACTOR1(0,2.0);
      // verify the command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,MathReceiverComponentBase::OPCODE_MR_SET_FACTOR1,10,Fw::COMMAND_OK);
      
      // clear the history
      this->clearHistory();

      // invoke operation port with add operation
      this->invoke_to_mathIn(0,2.0,3.0,Ref::MATH_MULTIPLY);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);

      // verify the result of the operation was returned
      F32 result = (2.0*3.0)*2.0/-1.0;
      // the event and telemetry channel use the Ref::MathOp type for values
      Ref::MathOp checkOp(2.0,3.0,Ref::MULT,result);

      // check that there was one and only one port invocation
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // check that only the port we expected was invoked
      ASSERT_from_mathOut_SIZE(1);
      // check that the component did the operation correctly. 
      ASSERT_from_mathOut(0,result);
      // verify telemetry and events

      // check that there was only one event
      ASSERT_EVENTS_SIZE(1);
      // check that it was the op event
      ASSERT_EVENTS_MR_OPERATION_PERFORMED_SIZE(1);
      // check that the event has the correct argument
      ASSERT_EVENTS_MR_OPERATION_PERFORMED(0,checkOp);

      // check that there was only one channel written
      ASSERT_TLM_SIZE(1);
      // check that it was the op channel
      ASSERT_TLM_MR_OPERATION_SIZE(1);
      // check for the correct value of the channel
      ASSERT_TLM_MR_OPERATION(0,checkOp);
  }

  void Tester ::
    testDivCommand(void) 
  {
      // set the test value for the parameter before loading - it will be initialized to this value
      this->paramSet_factor2(25.0,Fw::PARAM_VALID);

      // load parameters
      this->component.loadParameters();

      // send the command to set factor1 to 2.0
      this->sendCmd_MR_SET_FACTOR1(0,10,2.0);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);
      // verify the changed value events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1(0,2.0);
      ASSERT_TLM_MR_FACTOR1S_SIZE(1);
      ASSERT_TLM_MR_FACTOR1S(0,1);
      // verify the changed value channel
      ASSERT_TLM_SIZE(2);
      ASSERT_TLM_MR_FACTOR1_SIZE(1);
      ASSERT_TLM_MR_FACTOR1(0,2.0);
      // verify the command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,MathReceiverComponentBase::OPCODE_MR_SET_FACTOR1,10,Fw::COMMAND_OK);
      
      // clear the history
      this->clearHistory();

      // invoke operation port with add operation
      this->invoke_to_mathIn(0,2.0,3.0,Ref::MATH_DIVIDE);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);

      // verify the result of the operation was returned
      F32 result = (2.0/3.0)*2.0/25;
      // the event and telemetry channel use the Ref::MathOp type for values
      Ref::MathOp checkOp(2.0,3.0,Ref::DIVIDE,result);

      // check that there was one and only one port invocation
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // check that only the port we expected was invoked
      ASSERT_from_mathOut_SIZE(1);
      // check that the component did the operation correctly. 
      ASSERT_from_mathOut(0,result);
      // verify telemetry and events

      // check that there was only one event
      ASSERT_EVENTS_SIZE(1);
      // check that it was the op event
      ASSERT_EVENTS_MR_OPERATION_PERFORMED_SIZE(1);
      // check that the event has the correct argument
      ASSERT_EVENTS_MR_OPERATION_PERFORMED(0,checkOp);

      // check that there was only one channel written
      ASSERT_TLM_SIZE(1);
      // check that it was the op channel
      ASSERT_TLM_MR_OPERATION_SIZE(1);
      // check for the correct value of the channel
      ASSERT_TLM_MR_OPERATION(0,checkOp);
  }

  void Tester ::
    testThrottle(void) 
  {

      // send the number of commands required to throttle the event
      // Use the autocoded value so the unit test passes if the 
      // throttle value is changed
      for (NATIVE_UINT_TYPE cycle = 0; cycle < MathReceiverComponentBase::EVENTID_MR_SET_FACTOR1_THROTTLE; cycle++) {
          // send the command to set factor1 to 2.0
          this->sendCmd_MR_SET_FACTOR1(0,10,2.0);
          // invoke scheduler port to dispatch message
          this->invoke_to_SchedIn(0,0);
          // verify the changed value events
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_MR_SET_FACTOR1_SIZE(1);
          ASSERT_EVENTS_MR_SET_FACTOR1(0,2.0);
          // verify the changed value channel
          ASSERT_TLM_SIZE(2);
          ASSERT_TLM_MR_FACTOR1_SIZE(1);
          ASSERT_TLM_MR_FACTOR1(0,2.0);
          ASSERT_TLM_MR_FACTOR1S_SIZE(1);
          ASSERT_TLM_MR_FACTOR1S(0,cycle+1);
          
          // verify the command response was sent
          ASSERT_CMD_RESPONSE_SIZE(1);
          ASSERT_CMD_RESPONSE(0,MathReceiverComponentBase::OPCODE_MR_SET_FACTOR1,10,Fw::COMMAND_OK);
          
          // clear the history
          this->clearHistory();
      }

      // sending the command now will not result in an event since
      // the throttle value has been reached

      // send the command to set factor1 to 2.0
      this->sendCmd_MR_SET_FACTOR1(0,10,2.0);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);
      // verify the changed value events
      ASSERT_EVENTS_SIZE(0);
      // verify the changed value channel
      ASSERT_TLM_SIZE(2);
      ASSERT_TLM_MR_FACTOR1_SIZE(1);
      ASSERT_TLM_MR_FACTOR1(0,2.0);
      // verify the command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,MathReceiverComponentBase::OPCODE_MR_SET_FACTOR1,10,Fw::COMMAND_OK);
      
      // clear the history
      this->clearHistory();

      // send the command to clear the throttle
      this->sendCmd_MR_CLEAR_EVENT_THROTTLE(0,10);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);
      // verify clear event was sent
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_MR_THROTTLE_CLEARED_SIZE(1);
      
      // clear the history
      this->clearHistory();
      // sending the command will now produce the event again
      this->sendCmd_MR_SET_FACTOR1(0,10,2.0);
      // invoke scheduler port to dispatch message
      this->invoke_to_SchedIn(0,0);
      // verify the changed value event
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1_SIZE(1);
      ASSERT_EVENTS_MR_SET_FACTOR1(0,2.0);

  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_mathOut_handler(
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
    this->pushFromPortEntry_mathOut(result);
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

    // SchedIn
    this->connect_to_SchedIn(
        0,
        this->component.get_SchedIn_InputPort(0)
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

    // ParamGet
    this->component.set_ParamGet_OutputPort(
        0, 
        this->get_from_ParamGet(0)
    );

    // ParamSet
    this->component.set_ParamSet_OutputPort(
        0, 
        this->get_from_ParamSet(0)
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
