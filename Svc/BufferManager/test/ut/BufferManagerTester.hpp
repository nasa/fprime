// ======================================================================
// \title  BufferManager/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for BufferManager test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "BufferManagerGTestBase.hpp"
#include "Svc/BufferManager/BufferManagerComponentImpl.hpp"

namespace Svc {

  class BufferManagerTester :
    public BufferManagerGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object BufferManagerTester
      //!
      BufferManagerTester();

      //! Destroy object BufferManagerTester
      //!
      ~BufferManagerTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Test Setup
      //!
      void testSetup();

      //! One buffer size
      void oneBufferSize();

      //! Multiple buffer sizes
      void multBuffSize();

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
      BufferManagerComponentImpl component;

      void textLogIn(
          const FwEventIdType id, //!< The event ID
          const Fw::Time& timeTag, //!< The time
          const Fw::LogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
      ) override;


  };

} // end namespace Svc

#endif
