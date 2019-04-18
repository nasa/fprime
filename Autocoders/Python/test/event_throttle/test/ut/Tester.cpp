// ======================================================================
// \title  TestLog.hpp
// \author tcanham
// \brief  cpp file for TestLog test harness implementation class
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

namespace Somewhere {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) :
#if FW_OBJECT_NAMES == 1
      TestLogGTestBase("Tester", MAX_HISTORY_SIZE),
      component("TestLog")
#else
      TestLogGTestBase(MAX_HISTORY_SIZE),
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

  void Tester::doEventThrottleTest(void) {
      // should send up to EVENTID_SOMEEVENT_THROTTLE events
      for (NATIVE_UINT_TYPE call = 0; call < TestLogComponentBase::EVENTID_SOMEEVENT_THROTTLE; call++) {
          this->component.sendEvent(1, 2.0, 3);
          ASSERT_EVENTS_SIZE(call+1);
          ASSERT_EVENTS_SomeEvent_SIZE(call+1);
      }

      // should be throttled
      this->clearHistory();

      this->component.sendEvent(1, 2.0, 3);
      ASSERT_EVENTS_SIZE(0);
      ASSERT_EVENTS_SomeEvent_SIZE(0);

      this->component.resetEvent();

      // should start sending again
      for (NATIVE_UINT_TYPE call = 0; call < TestLogComponentBase::EVENTID_SOMEEVENT_THROTTLE; call++) {
          this->component.sendEvent(1, 2.0, 3);
          ASSERT_EVENTS_SIZE(call+1);
          ASSERT_EVENTS_SomeEvent_SIZE(call+1);
      }

      // should be throttled
      this->clearHistory();

      this->component.sendEvent(1, 2.0, 3);
      ASSERT_EVENTS_SIZE(0);
      ASSERT_EVENTS_SomeEvent_SIZE(0);

  }


  void Tester::textLogIn(
                  const FwEventIdType id, //!< The event ID
                  Fw::Time& timeTag, //!< The time
                  const Fw::TextLogSeverity severity, //!< The severity
                  const Fw::TextLogString& text //!< The event string
              ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e,stdout);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void)
  {

    // aport
    this->connect_to_aport(
        0,
        this->component.get_aport_InputPort(0)
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
        INSTANCE
    );
  }

} // end namespace Somewhere
