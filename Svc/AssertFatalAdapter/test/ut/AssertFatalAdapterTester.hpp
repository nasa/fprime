// ======================================================================
// \title  AssertFatalAdapter/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for AssertFatalAdapter test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "AssertFatalAdapterGTestBase.hpp"
#include "Svc/AssertFatalAdapter/AssertFatalAdapterComponentImpl.hpp"

namespace Svc {

  class AssertFatalAdapterTester :
    public AssertFatalAdapterGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object AssertFatalAdapterTester
      //!
      AssertFatalAdapterTester();

      //! Destroy object AssertFatalAdapterTester
      //!
      ~AssertFatalAdapterTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!
      void testAsserts();

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

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      AssertFatalAdapterComponentImpl component;

      void textLogIn(
                const FwEventIdType id, //!< The event ID
                const Fw::Time& timeTag, //!< The time
                const Fw::LogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
            ) override;


  };

} // end namespace Svc

#endif
