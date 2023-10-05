// ======================================================================
// \title  LinuxTimer.hpp
// \author tim
// \brief  cpp file for LinuxTimer test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "LinuxTimerTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  LinuxTimerTester ::
    LinuxTimerTester() :
      LinuxTimerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("LinuxTimer")
      ,m_numCalls(0)
  {
    this->initComponents();
    this->connectPorts();
  }

  LinuxTimerTester ::
    ~LinuxTimerTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void LinuxTimerTester ::
      runCycles()
  {
    this->m_numCalls = 5;
    this->component.startTimer(1000);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void LinuxTimerTester ::
    from_CycleOut_handler(
        const NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
      printf("TICK\n");

      if (--this->m_numCalls == 0) {
          this->component.quit();
      }
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void LinuxTimerTester ::
    connectPorts()
  {

    // CycleOut
    this->component.set_CycleOut_OutputPort(
        0,
        this->get_from_CycleOut(0)
    );




  }

  void LinuxTimerTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
