// ======================================================================
// \title  SignalGen/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for SignalGen test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "SignalGenGTestBase.hpp"
#include "Ref/SignalGen/SignalGen.hpp"

namespace Ref {

  class SignalGenTester :
    public SignalGenGTestBase
  {
      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:
      // Maximum size of histories storing events, telemetry, and port outputs
      static const U32 MAX_HISTORY_SIZE = 10;
      // Instance ID supplied to the component instance under test
      static const FwEnumStoreType TEST_INSTANCE_ID = 0;
      // Queue depth supplied to component instance under test
      static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

      //! Construct object SignalGenTester
      //!
      SignalGenTester();

      //! Destroy object SignalGenTester
      //!
      ~SignalGenTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!
      void test_start();

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

      //! Handle a data product get from the component under test
      //!
      //! By default, (1) call pushProductGetEntry; (2) do not allocate a buffer
      //! and return FAILURE. You can override this behavior, e.g., to call
      //! pushProductGetEntry, allocate a buffer and return SUCCESS.
      virtual Fw::Success::T productGet_handler(
        FwDpIdType id, //!< The container ID (input)
        FwSizeType dataSize, //!< The data size of the requested buffer (input)
        Fw::Buffer& buffer //!< The buffer (output)
      );


      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      SignalGen component;

      void textLogIn(
        FwEventIdType id, //!< The event ID
        const Fw::Time& timeTag, //!< The time
        const Fw::LogSeverity severity, //!< The severity
        const Fw::TextLogString& text //!< The event string
      ) override;

      U8 m_dpBuff[1024];
      Fw::Buffer m_reqDpBuff;

  };

} // end namespace Ref

#endif
