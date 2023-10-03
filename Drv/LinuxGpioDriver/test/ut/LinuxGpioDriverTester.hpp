// ======================================================================
// \title  LinuxGpioDriver/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for LinuxGpioDriver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "TesterBase.hpp"
#include "Drv/LinuxGpioDriver/LinuxGpioDriverComponentImpl.hpp"

namespace Drv {

  class LinuxGpioDriverTester :
    public LinuxGpioDriverTesterBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object LinuxGpioDriverTester
      //!
      LinuxGpioDriverTester();

      //! Destroy object LinuxGpioDriverTester
      //!
      ~LinuxGpioDriverTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Interrupt testing
      //!
      void testInterrupt(NATIVE_INT_TYPE gpio, NATIVE_INT_TYPE cycles);

      //! Test output
      void testOutput(NATIVE_INT_TYPE gpio, NATIVE_INT_TYPE cycles);

      //! Test input
      void testInput(NATIVE_INT_TYPE gpio, NATIVE_INT_TYPE cycles);

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_intOut
      //!
      void from_intOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Svc::TimerVal &cycleStart /*!< Cycle start timer value*/
      );

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
      LinuxGpioDriverComponentImpl component;

      void textLogIn(
                const FwEventIdType id, //!< The event ID
                Fw::Time& timeTag, //!< The time
                const Fw::TextLogSeverity severity, //!< The severity
                const Fw::TextLogString& text //!< The event string
            );

      NATIVE_INT_TYPE m_cycles; //!< cycles to wait
      NATIVE_INT_TYPE m_currCycle; //!< curr cycle in test

  };

} // end namespace Drv

#endif
