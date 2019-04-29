// ======================================================================
// \title  LinuxSpiDriver/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for LinuxSpiDriver test harness implementation class
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
#include "Drv/LinuxSpiDriver/LinuxSpiDriverComponentImpl.hpp"

namespace Drv {

  class Tester :
    public LinuxSpiDriverTesterBase
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

      //! To do
      //!
      void sendBuffer(U8* buffer, NATIVE_INT_TYPE size);

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

      void textLogIn(const FwEventIdType id, //!< The event ID
                Fw::Time& timeTag, //!< The time
                const Fw::TextLogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
                );


    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      LinuxSpiDriverComponentImpl component;

  };

} // end namespace Drv

#endif
