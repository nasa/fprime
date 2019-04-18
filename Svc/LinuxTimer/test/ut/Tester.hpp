// ====================================================================== 
// \title  LinuxTimer/test/ut/Tester.hpp
// \author tim
// \brief  hpp file for LinuxTimer test harness implementation class
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
#include "Svc/LinuxTimer/LinuxTimerComponentImpl.hpp"

namespace Svc {

  class Tester :
    public LinuxTimerGTestBase
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
      void runCycles(void);

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_CycleOut
      //!
      void from_CycleOut_handler(
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
      LinuxTimerComponentImpl component;

      NATIVE_INT_TYPE m_numCalls;

  };

} // end namespace Svc

#endif
