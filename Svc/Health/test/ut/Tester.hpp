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

#include "GTestBase.hpp"
#include "Svc/Health/HealthComponentImpl.hpp"

namespace Svc {

  class Tester :
    public HealthGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object Tester
      //!
      Tester(void);

      //! Destroy object Tester
      //!
      ~Tester(void);

    public:

      // ---------------------------------------------------------------------- 
      // Tests
      // ---------------------------------------------------------------------- 

      void nominalTlm(void);
      void warningTlm(void);
      void faultTlm(void);
      void disableAllMonitoring(void);
      void disableOneMonitoring(void);
      void updatePingTimeout(void);
      void watchdogCheck(void);
      void nominalCmd(void);
      void nominal2CmdsDuringTlm(void);
      void miscellaneous(void);

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_PingSend
      //!
      void from_PingSend_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 key //!< Value to return to pinger
      );

      //! Handler for from_WdogStroke
      //!
      void from_WdogStroke_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 code //!< Watchdog stroke code
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts(void);

      //! Initialize components
      //!
      void initComponents(void);

      void dispatchAll(void);

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
                Fw::Time& timeTag, //!< The time
                const Fw::TextLogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
                );

  };

} // end namespace Svc

#endif
