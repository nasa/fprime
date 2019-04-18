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

#include "Tester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction 
  // ----------------------------------------------------------------------

  Tester ::
    Tester(void) : 
#if FW_OBJECT_NAMES == 1
      LinuxTimerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("LinuxTimer")
#else
      LinuxTimerGTestBase(MAX_HISTORY_SIZE),
      component()
#endif
      ,m_numCalls(0)
  {
    this->initComponents();
    this->connectPorts();
  }

  Tester ::
    ~Tester(void) 
  {
    
  }

  // ----------------------------------------------------------------------
  // Tests 
  // ----------------------------------------------------------------------

  void Tester ::
      runCycles(void)
  {
    this->m_numCalls = 5;
    this->component.startTimer(1000);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
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

  void Tester ::
    connectPorts(void) 
  {

    // CycleOut
    this->component.set_CycleOut_OutputPort(
        0, 
        this->get_from_CycleOut(0)
    );




  }

  void Tester ::
    initComponents(void) 
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
