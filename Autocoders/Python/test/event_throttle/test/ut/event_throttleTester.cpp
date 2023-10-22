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

#include "event_throttleTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Somewhere {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  event_throttleTester ::
    event_throttleTester() :
      TestLogGTestBase("Tester", MAX_HISTORY_SIZE),
      component("TestLog")
  {
    this->initComponents();
    this->connectPorts();
  }

  event_throttleTester ::
    ~event_throttleTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void event_throttleTester::doEventThrottleTest() {
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


  void event_throttleTester::textLogIn(
                  const FwEventIdType id, //!< The event ID
                  Fw::Time& timeTag, //!< The time
                  const Fw::LogSeverity severity, //!< The severity
                  const Fw::TextLogString& text //!< The event string
              ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e,stdout);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void event_throttleTester ::
    connectPorts()
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

  void event_throttleTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Somewhere
