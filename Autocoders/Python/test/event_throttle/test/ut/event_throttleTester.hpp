// ======================================================================
// \title  TestLog/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for TestLog test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "event_throttleGTestBase.hpp"
#include "Autocoders/Python/test/event_throttle/TestLogImpl.hpp"

namespace Somewhere {

  class event_throttleTester :
    public TestLogGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object event_throttleTester
      //!
      event_throttleTester();

      //! Destroy object event_throttleTester
      //!
      ~event_throttleTester();


    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! test event throttling
      void doEventThrottleTest();

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

    private:

      void textLogIn(
                const FwEventIdType id, //!< The event ID
                Fw::Time& timeTag, //!< The time
                const Fw::LogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
            );
      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      TestLogImpl component;

  };

} // end namespace Somewhere

#endif
