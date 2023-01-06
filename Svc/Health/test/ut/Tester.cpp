// ======================================================================
// \title  Health.hpp
// \author jdperez
// \brief  cpp file for Health test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"
#include <Fw/Test/UnitTest.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE (Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS * Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS * Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS)
#define QUEUE_DEPTH (Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2)
#define FLAG_KEY_VALUE 0xcafecafe

static_assert(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS < 0xcafecafe, "");

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester() :
      HealthGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Health")
  {
    this->connectPorts();
    this->initComponents();
  }

  Tester ::
    ~Tester()
  {

  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_PingSend_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    if(this->override) {
        invoke_to_PingReturn(portNum,this->override_key);
    } else {

        ASSERT_TRUE(portNum < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);

        if (key == this->keys[portNum]) {
            this->invoke_to_PingReturn(portNum, key);
        }

    }

    this->pushFromPortEntry_PingSend(key);
  }

  void Tester ::
    from_WdogStroke_handler(
        const NATIVE_INT_TYPE portNum,
        U32 code
    )
  {
    this->pushFromPortEntry_WdogStroke(code);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // PingReturn
    for (NATIVE_INT_TYPE i = 0; i < this->component.getNum_PingReturn_InputPorts(); ++i) {
        this->connect_to_PingReturn(
            i,
            this->component.get_PingReturn_InputPort(i)
        );
    }

    // Run
    this->connect_to_Run(
        0,
        this->component.get_Run_InputPort(0)
    );

    // CmdDisp
    this->connect_to_CmdDisp(
        0,
        this->component.get_CmdDisp_InputPort(0)
    );

    // PingSend
    for (NATIVE_INT_TYPE i = 0; i < this->component.getNum_PingSend_OutputPorts(); ++i) {
      this->component.set_PingSend_OutputPort(
          i,
          this->get_from_PingSend(i)
      );
    }

    // WdogStroke
    this->component.set_WdogStroke_OutputPort(
        0,
        this->get_from_WdogStroke(0)
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

    this->numPingEntries = Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS;
    this->watchDogCode = 42;


    this->component.init(
        QUEUE_DEPTH, INSTANCE
    );

    for (NATIVE_UINT_TYPE entry = 0; entry < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; entry++) {
        this->pingEntries[entry].warnCycles = Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS+entry;
        this->pingEntries[entry].fatalCycles = Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2+entry+1;
        this->pingEntries[entry].entryName.format("task%d",entry);
    }

    this->component.setPingEntries(this->pingEntries, this->numPingEntries, this->watchDogCode);
    this->override = false;
    this->override_key = 0;

    for (U32 i = 0; i < this->numPingEntries; i++) {
        this->keys[i] = 100000;
    }
  }

  void Tester ::
  	  dispatchAll()
  {
	  HealthComponentBase::MsgDispatchStatus stat = HealthComponentBase::MSG_DISPATCH_OK;
      while (HealthComponentBase::MSG_DISPATCH_OK == stat) {
          stat = this->component.doDispatch();
          FW_ASSERT(stat != HealthComponentBase::MSG_DISPATCH_ERROR);
      }
  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
  nominalTlm()
  {
	  TEST_CASE(900.1.1,"Nominal Telemetry");
	  REQUIREMENT("ISF-HTH-001");
	  COMMENT("The Svc::Health component shall ping each output port specified in the provided table.");

	  //Check initial state is as expected
	  ASSERT_EVENTS_SIZE(0);
	  ASSERT_TLM_SIZE(0);
	  ASSERT_CMD_RESPONSE_SIZE(0);

	  for (NATIVE_UINT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
	      this->keys[port] = port;
	  }

	  //invoke run handler same number as number of ports
	  for (U32 i = 0; i < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; i++) {
		  this->invoke_to_Run(0,0);
	      for (NATIVE_UINT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
	          this->keys[port] += Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS;
	      }

		  // Check no events or telemetry have occurred
		  ASSERT_EVENTS_SIZE(0);
		  ASSERT_TLM_SIZE(0);
		  ASSERT_CMD_RESPONSE_SIZE(0);
	  }

	  //Check no events or telemetry have occurred
	  ASSERT_EVENTS_SIZE(0);
	  ASSERT_TLM_SIZE(0);
	  ASSERT_CMD_RESPONSE_SIZE(0);

  }

  void Tester ::
  warningTlm()
  {
      TEST_CASE(900.1.2,"Warning Telemetry");
      REQUIREMENT("ISF-HTH-002");
      COMMENT("The Svc::Health component shall track the timeout cycles for each component.");
      //Check initial state is as expected
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);
      ASSERT_CMD_RESPONSE_SIZE(0);
      return;

      //invoke run handler enough times for warnings
      for (U32 i = 0; i < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2; i++) {
          this->invoke_to_Run(0,0);
          for (NATIVE_UINT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
              ASSERT_EQ(i+1,this->component.m_pingTrackerEntries[port].cycleCount);
          }
      }

      //check for expected warning EVRs from each entry
      ASSERT_EVENTS_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      ASSERT_TLM_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      ASSERT_EVENTS_HLTH_PING_WARN_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);

      for (NATIVE_UINT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
          char name[80];
          snprintf(name, sizeof(name), "task%d",port);
          ASSERT_EVENTS_HLTH_PING_WARN(port,name);
      }

  }


  void Tester ::
  faultTlm()
  {
      TEST_CASE(900.1.3,"Fault Telemetry");
      REQUIREMENT("ISF-HTH-003");
      COMMENT("The Svc::Health component shall issue a FATAL event if a component fails to return a ping by the specified timeout.");

      //Check initial state is as expected
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);
      ASSERT_CMD_RESPONSE_SIZE(0);

      //invoke run handler enough times for warnings
      for (U32 i = 0; i < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2; i++) {
          this->invoke_to_Run(0,0);
          for (NATIVE_UINT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
              ASSERT_EQ(i+1,this->component.m_pingTrackerEntries[port].cycleCount);
          }
      }

      //check for expected warning EVRs from each entry
      ASSERT_EVENTS_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      ASSERT_TLM_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      ASSERT_EVENTS_HLTH_PING_WARN_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);

      for (NATIVE_UINT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
          char name[80];
          snprintf(name, sizeof(name), "task%d",port);
          ASSERT_EVENTS_HLTH_PING_WARN(port,name);
      }

      this->clearEvents();
      this->clearTlm();

      //invoke run handler enough times for FATALs
      for (U32 i = 0; i < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; i++) {
          this->invoke_to_Run(0,0);
          for (NATIVE_UINT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
              ASSERT_EQ(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2+i+1,this->component.m_pingTrackerEntries[port].cycleCount);
          }
      }
      this->invoke_to_Run(0,0);

      //check for expected warning EVRs from each entry
      ASSERT_EVENTS_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      ASSERT_EVENTS_HLTH_PING_LATE_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);

      for (NATIVE_UINT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
          char name[80];
          snprintf(name, sizeof(name), "task%d",port);
          ASSERT_EVENTS_HLTH_PING_LATE(port,name);
      }

  }

  void Tester ::
  disableAllMonitoring()
  {
      TEST_CASE(900.1.4,"Enable/Disable all monitoring");
      REQUIREMENT("ISF-HTH-004");
      COMMENT("The Svc::Health component shall have a command to enable or disable all monitoring.");

      //Check initial state is as expected
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);
      ASSERT_CMD_RESPONSE_SIZE(0);

      //invoke run handler
      for (U32 i = 0; i < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2; i++) {
          this->invoke_to_Run(0,0);
          for (NATIVE_INT_TYPE entry = 0; entry < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; entry++) {
              ASSERT_EQ(i+1,this->component.m_pingTrackerEntries[entry].cycleCount);
          }
      }

      ASSERT_EVENTS_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      ASSERT_EVENTS_HLTH_PING_WARN_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      for (NATIVE_INT_TYPE entry = 0; entry < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; entry++) {
          char name[80];
          snprintf(name, sizeof(name), "task%d",entry);
          ASSERT_EVENTS_HLTH_PING_WARN(entry, name);
      }

      ASSERT_TLM_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);

      this->clearEvents();
      this->clearTlm();

      //disable all monitoring
      this->sendCmd_HLTH_ENABLE(0,0,Fw::Enabled::DISABLED);
      this->dispatchAll();

      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_HLTH_CHECK_ENABLE_SIZE(1);
      ASSERT_EVENTS_HLTH_CHECK_ENABLE(0,Fw::Enabled::DISABLED);

      ASSERT_EQ(this->component.m_enabled, Fw::Enabled::DISABLED);

      //invoke run handler 100 times
      for (U32 i = 0; i < 100; i++) {
          this->invoke_to_Run(0,0);
          // cycle count should stay the same

          ASSERT_EQ(static_cast<U32>(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS)*2,
                  this->component.m_pingTrackerEntries[0].cycleCount);
      }

      //confirm no telemetry was received
      ASSERT_TLM_SIZE(0);

      this->clearEvents();
      this->clearTlm();

     //enable all monitoring
      this->sendCmd_HLTH_ENABLE(0,0,Fw::Enabled::ENABLED);
      this->dispatchAll();

      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_HLTH_CHECK_ENABLE_SIZE(1);
      ASSERT_EVENTS_HLTH_CHECK_ENABLE(0,Fw::Enabled::ENABLED);

      ASSERT_EQ(this->component.m_enabled, Fw::Enabled::ENABLED);

      this->clearEvents();
      this->clearTlm();
      // check that cycle counts increase again
      for (U32 i = 0; i < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; i++) {
          this->invoke_to_Run(0,0);
          for (NATIVE_INT_TYPE entry = 0; entry < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; entry++) {
              ASSERT_EQ(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2+1+i,this->component.m_pingTrackerEntries[entry].cycleCount);
          }
      }
      this->invoke_to_Run(0,0);

      // Should be FATAL timeouts
      ASSERT_EVENTS_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      ASSERT_EVENTS_HLTH_PING_LATE_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      for (NATIVE_INT_TYPE entry = 0; entry < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; entry++) {
          char name[80];
          snprintf(name,sizeof(name), "task%d",entry);
          ASSERT_EVENTS_HLTH_PING_LATE(entry,name);
      }

  }

  void Tester ::
  disableOneMonitoring()
  {
      TEST_CASE(900.1.5,"Enable/Disable individual monitors");
      REQUIREMENT("ISF-HTH-005");
      COMMENT("The Svc::Health component shall have a command to enable or disable monitoring for a particular port.");

      //Check initial state is as expected
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);
      ASSERT_CMD_RESPONSE_SIZE(0);

      // disable monitoring one at a time

      for (NATIVE_INT_TYPE entry = 0; entry < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; entry++) {
          this->clearEvents();
          this->clearHistory();
          // reset cycle count
          for (NATIVE_INT_TYPE e2 = 0; e2 < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; e2++) {
              this->component.m_pingTrackerEntries[e2].cycleCount = 0;
          }
          // disable entry
          char name[80];
          snprintf(name, sizeof(name), "task%d",entry);
          Fw::CmdStringArg task(name);
          this->sendCmd_HLTH_PING_ENABLE(0,10,name,Fw::Enabled::DISABLED);
          this->dispatchAll();
          ASSERT_CMD_RESPONSE_SIZE(1);
          ASSERT_CMD_RESPONSE(0,HealthComponentBase::OPCODE_HLTH_PING_ENABLE,10,Fw::CmdResponse::OK);

          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_HLTH_CHECK_PING(0,Fw::Enabled::DISABLED,name);

          // run a few cycles
          for (NATIVE_INT_TYPE cycle = 0; cycle < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; cycle++) {

              // run a cycle
              this->invoke_to_Run(0,0);

              // verify only enable entries count up
              for (NATIVE_INT_TYPE e3 = 0; e3 < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; e3++) {
                  if (e3 == entry) {
                      // shouldn't be counting up
                      ASSERT_EQ(0u,this->component.m_pingTrackerEntries[e3].cycleCount);
                  } else {
                      // others should be counting up
                      ASSERT_EQ(static_cast<U32>(cycle+1),this->component.m_pingTrackerEntries[e3].cycleCount);
                  }
              }
          }

          this->clearEvents();
          this->clearHistory();

          this->sendCmd_HLTH_PING_ENABLE(0,10,name,Fw::Enabled::ENABLED);
          this->dispatchAll();
          ASSERT_CMD_RESPONSE_SIZE(1);
          ASSERT_CMD_RESPONSE(0,HealthComponentBase::OPCODE_HLTH_PING_ENABLE,10,Fw::CmdResponse::OK);

          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_HLTH_CHECK_PING(0,Fw::Enabled::ENABLED,name);
      }
  }

  void Tester ::
  updatePingTimeout()
  {
      TEST_CASE(900.1.6,"Update ping timeouts");
      REQUIREMENT("ISF-HTH-006");
      COMMENT("The Svc::Health component shall have a command to update ping timeout values for a port");

      //Check initial state is as expected
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);
      ASSERT_CMD_RESPONSE_SIZE(0);

      for (NATIVE_UINT_TYPE entry = 0; entry < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; entry++) {
          this->clearEvents();
          this->clearHistory();
          // verify previous value
          ASSERT_EQ(this->pingEntries[entry].warnCycles,Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS+entry);
          ASSERT_EQ(this->pingEntries[entry].fatalCycles,Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2+entry+1);
          char name[80];
          snprintf(name, sizeof(name), "task%d",entry);
          Fw::CmdStringArg task(name);
          this->sendCmd_HLTH_CHNG_PING(0,10,task,
                  Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*4+entry,
                  Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*5+entry);
          this->dispatchAll();
          // verify EVR
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_HLTH_PING_UPDATED_SIZE(1);
          ASSERT_EVENTS_HLTH_PING_UPDATED(0,name,
                  Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*4+entry,
                  Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*5+entry);
          ASSERT_EQ(this->component.m_pingTrackerEntries[entry].entry.warnCycles,
                  Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*4+entry);
          ASSERT_EQ(this->component.m_pingTrackerEntries[entry].entry.fatalCycles,
                  Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*5+entry);
      }
  }

  void Tester ::
  watchdogCheck()
  {
      TEST_CASE(900.1.7,"Watchdog check");
      REQUIREMENT("ISF-HTH-007");
      COMMENT("The Svc::Health component shall stroke a watchdog port while all ping replies are within their limit and health checks pass");

      //Check initial state is as expected
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);
      ASSERT_CMD_RESPONSE_SIZE(0);

      //invoke run handler 10 times and check watchdog port output
      for (U32 i = 0; i < 10; i++) {
          this->invoke_to_Run(0,0);
          ASSERT_from_WdogStroke_SIZE(i+1);
          ASSERT_from_WdogStroke(i, this->component.m_watchDogCode);
          //increment watchdog code
          this->component.m_watchDogCode += 1;
      }
  }

  void Tester ::
  nominalCmd()
  {
	  TEST_CASE(900.1.8,"Nominal Command");
	  COMMENT("Process command during quiescent (no telemetry readout) period.");

	  Fw::LogStringArg arg = "task0";
	  Fw::CmdStringArg carg = arg;

	  ASSERT_EVENTS_SIZE(0);
	  ASSERT_TLM_SIZE(0);
	  ASSERT_CMD_RESPONSE_SIZE(0);

	  sendCmd_HLTH_ENABLE(0,0, Fw::Enabled::DISABLED);
	  this->dispatchAll();
	  ASSERT_EQ(Fw::Enabled(Fw::Enabled::DISABLED), this->component.m_enabled);
	  ASSERT_EVENTS_SIZE(1);
	  ASSERT_EVENTS_HLTH_CHECK_ENABLE(0,Fw::Enabled::DISABLED);
      this->clearEvents();

	  sendCmd_HLTH_ENABLE(0,0, Fw::Enabled::ENABLED);
	  this->dispatchAll();
	  ASSERT_EQ(Fw::Enabled(Fw::Enabled::ENABLED), this->component.m_enabled);
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_HLTH_CHECK_ENABLE(0,Fw::Enabled::ENABLED);

      this->clearEvents();
	  sendCmd_HLTH_PING_ENABLE(0, 0, carg, Fw::Enabled::DISABLED);
	  this->dispatchAll();
	  ASSERT_EQ(Fw::Enabled(Fw::Enabled::DISABLED), this->component.m_pingTrackerEntries[0].enabled);
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_HLTH_CHECK_PING(0,Fw::Enabled::DISABLED,"task0");

      this->clearEvents();
	  U32 warn_val = 9;
	  U32 fatal_val = 99;
	  sendCmd_HLTH_CHNG_PING(0, 0, carg, warn_val, fatal_val);
	  this->dispatchAll();

	  ASSERT_EQ(warn_val, this->component.m_pingTrackerEntries[0].entry.warnCycles);
	  ASSERT_EQ(fatal_val, this->component.m_pingTrackerEntries[0].entry.fatalCycles);

	  //Check no events or telemetry have occurred
	  ASSERT_EVENTS_SIZE(1);
	  ASSERT_EVENTS_HLTH_PING_UPDATED_SIZE(1);
	  ASSERT_EVENTS_HLTH_PING_UPDATED(0,"task0",warn_val,fatal_val);
	  ASSERT_TLM_SIZE(0);
  }


  void Tester ::
  nominal2CmdsDuringTlm()
  {
	  TEST_CASE(900.1.9,"Nominal 2 commands called during telemetry readouts.");
	  COMMENT("Process commands during busy (telemetry readout) period.");

	  //Check no events or telemetry have occurred
	  ASSERT_EVENTS_SIZE(0);
	  ASSERT_TLM_SIZE(0);

	  for (NATIVE_INT_TYPE entry = 0; entry < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; entry++) {
	      this->keys[entry] = entry;
	  }

	  this->invoke_to_Run(0,0);

	  //Check no events or telemetry have occurred
	  ASSERT_EVENTS_SIZE(0);
	  ASSERT_TLM_SIZE(0);

	  sendCmd_HLTH_ENABLE(0,0, Fw::Enabled::DISABLED);
	  this->dispatchAll();
	  ASSERT_EQ(Fw::Enabled(Fw::Enabled::DISABLED), this->component.m_enabled);

	  return;
	  //invoke schedIn handler for 50 times
	  for (U32 i = 0; i < 50; i++) {
	      printf("Invoke: %d\n",i);
	      this->invoke_to_Run(0,0);

	      //Check no events or telemetry have occurred
	      ASSERT_EVENTS_SIZE(0);
	      ASSERT_TLM_SIZE(0);
	  }

	  sendCmd_HLTH_ENABLE(0,0, Fw::Enabled::ENABLED);
	  this->dispatchAll();
	  ASSERT_EQ(Fw::Enabled(Fw::Enabled::ENABLED), this->component.m_enabled);

	  this->invoke_to_Run(0,0);

	  //Check no events or telemetry have occurred
	  ASSERT_EVENTS_SIZE(0);
	  ASSERT_TLM_SIZE(0);
  }


  void Tester ::
  miscellaneous()
  {
      TEST_CASE(900.1.10,"Miscellaneous remaining tests.");
      COMMENT("Case 1: Ping port anomalies.");

      //Check no events or telemetry have occurred
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);

      //send a bad ping return key
      this->override = true;
      this->override_key = FLAG_KEY_VALUE;

      //invoke schedIn
      this->invoke_to_Run(0,0);
      this->dispatchAll();

      //check for expected EVRs
      ASSERT_EVENTS_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      ASSERT_EVENTS_HLTH_PING_WRONG_KEY_SIZE(Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS);
      for (NATIVE_INT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS; port++) {
          char name[80];
          snprintf(name, sizeof(name), "task%d",port);
          ASSERT_EVENTS_HLTH_PING_WRONG_KEY(port,name,FLAG_KEY_VALUE);
      }

      this->clearEvents();
      this->clearTlm();

      COMMENT("Case 2: Command input anomalies.");

      //Check no events or telemetry have occurred
      ASSERT_EVENTS_SIZE(0);
      ASSERT_TLM_SIZE(0);

      //send command with bad value
      Fw::Enabled badValue = static_cast<Fw::Enabled::t>(Fw::Enabled::NUM_CONSTANTS);
      sendCmd_HLTH_PING_ENABLE(0,0,"task0",badValue);
      this->dispatchAll();

      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,HealthComponentBase::OPCODE_HLTH_PING_ENABLE,0,Fw::CmdResponse::FORMAT_ERROR);

      //send command with bad ping entry
      sendCmd_HLTH_PING_ENABLE(0,0,"notask",Fw::Enabled::ENABLED);
      this->dispatchAll();

      ASSERT_CMD_RESPONSE_SIZE(2);
      ASSERT_CMD_RESPONSE(1,HealthComponentBase::OPCODE_HLTH_PING_ENABLE,0,Fw::CmdResponse::VALIDATION_ERROR);
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_HLTH_CHECK_LOOKUP_ERROR_SIZE(1);
      ASSERT_EVENTS_HLTH_CHECK_LOOKUP_ERROR(0,"notask");

      this->clearHistory();

      //send update timeout command with bad thresholds
      sendCmd_HLTH_CHNG_PING(0, 0,"task0", 10, 9);
      this->dispatchAll();

      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,HealthComponentBase::OPCODE_HLTH_CHNG_PING,0,Fw::CmdResponse::VALIDATION_ERROR);
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_HLTH_PING_INVALID_VALUES_SIZE(1);
      ASSERT_EVENTS_HLTH_PING_INVALID_VALUES(0,"task0",10,9);

      this->clearEvents();
      this->clearHistory();

      //send update timeout command with ping entry
      sendCmd_HLTH_CHNG_PING(0, 0, "sometask", 1, 2);
      this->dispatchAll();

      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(0,HealthComponentBase::OPCODE_HLTH_CHNG_PING,0,Fw::CmdResponse::VALIDATION_ERROR);

      this->clearEvents();
      this->clearHistory();

      COMMENT("Case 3: All ping ports except one respond to ping requests.");

      ASSERT_EVENTS_SIZE(0);

      //set up the correct keys for all ports except last
      for (NATIVE_INT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS - 1; port++) {
          this->keys[port] = port;
      }
      this->keys[Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS - 1] = 9999;

      this->override = false;
      this->component.m_key = 0;

      //reset cycle counts
      for (U32 i = 0; i < this->numPingEntries; i++) {
          this->component.m_pingTrackerEntries[i].cycleCount = 0;
      }

      //invoke schedIn handler
      for (U32 i = 0; i < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS*2; i++) {
          //invoke schedIn
          this->invoke_to_Run(0,0);

          for (NATIVE_INT_TYPE port = 0; port < Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS - 1; port++) {
              if (i == 0) {
                  this->keys[port] += Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS;
              } else {
                  this->keys[port] += Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS-1;
              }
          }
      }

      //check that only the last ping entry caused a warning EVR due to no ping response
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_HLTH_PING_WARN_SIZE(1);
      char name[80];
      snprintf(name, sizeof(name), "task%d",Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS-1);
      ASSERT_EVENTS_HLTH_PING_WARN(0,name);
      ASSERT_TLM_SIZE(1);
      ASSERT_TLM_PingLateWarnings_SIZE(1);
      ASSERT_TLM_PingLateWarnings(0,1);

  }

  void Tester::textLogIn(const FwEventIdType id, //!< The event ID
          Fw::Time& timeTag, //!< The time
          const Fw::LogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
          ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e, stdout);
  }

} // end namespace Svc
