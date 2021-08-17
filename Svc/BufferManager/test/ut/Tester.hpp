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

#include "GTestBase.hpp"
#include "Svc/BufferManager/BufferManagerComponentImpl.hpp"

namespace Svc {

  class Tester :
    public BufferManagerGTestBase
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

      //! Test Setup
      //!
      void testSetup(void);

      //! One buffer size
      void oneBufferSize(void);

      //! Multiple buffer sizes
      void multBuffSize(void);

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

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      BufferManagerComponentImpl component;

      void textLogIn(
          const FwEventIdType id, //!< The event ID
          Fw::Time& timeTag, //!< The time
          const Fw::TextLogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
      );


  };

} // end namespace Svc

#endif
