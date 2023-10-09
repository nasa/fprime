// ======================================================================
// \title  Health/test/ut/Tester.hpp
// \author jdperez
// \brief  hpp file for Health test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "HealthGTestBase.hpp"
#include "Svc/Health/HealthComponentImpl.hpp"

namespace Svc {

  class HealthTester :
    public HealthGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object HealthTester
      //!
      HealthTester();

      //! Destroy object HealthTester
      //!
      ~HealthTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      void nominalTlm();
      void warningTlm();
      void faultTlm();
      void disableAllMonitoring();
      void disableOneMonitoring();
      void updatePingTimeout();
      void watchdogCheck();
      void nominalCmd();
      void nominal2CmdsDuringTlm();
      void miscellaneous();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_PingSend
      //!
      void from_PingSend_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 key //!< Value to return to pinger
      ) override;

      //! Handler for from_WdogStroke
      //!
      void from_WdogStroke_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 code //!< Watchdog stroke code
      ) override;

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

      void dispatchAll();

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      NATIVE_UINT_TYPE numPingEntries;
      HealthImpl::PingEntry pingEntries[Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS];
      U32 watchDogCode;
      U32 keys[Svc::HealthComponentBase::NUM_PINGSEND_OUTPUT_PORTS];
      bool override;
      U32 override_key;

      //! The component under test
      //!
      HealthImpl component;

      void textLogIn(const FwEventIdType id, //!< The event ID
                const Fw::Time& timeTag, //!< The time
                const Fw::LogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
                ) override;

  };

} // end namespace Svc

#endif
