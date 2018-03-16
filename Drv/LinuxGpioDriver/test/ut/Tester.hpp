// ======================================================================
// \title  LinuxGpioDriver/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for LinuxGpioDriver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
//
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "TesterBase.hpp"
#include "Drv/LinuxGpioDriver/LinuxGpioDriverComponentImpl.hpp"

namespace Drv {

  class Tester :
    public LinuxGpioDriverTesterBase
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

      //! Interrupt testing
      //!
      void testInterrrupt(NATIVE_INT_TYPE gpio, NATIVE_INT_TYPE cycles);

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
