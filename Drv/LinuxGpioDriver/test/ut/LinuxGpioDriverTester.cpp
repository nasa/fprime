// ======================================================================
// \title  LinuxGpioDriver.hpp
// \author tcanham
// \brief  cpp file for LinuxGpioDriver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "LinuxGpioDriverTester.hpp"
#include <Os/IntervalTimer.hpp>

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Drv {

  Os::IntervalTimer timer;
  U32 timerDiffList[100];
  U32 timerDiffIdx = 0;

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  LinuxGpioDriverTester ::
    LinuxGpioDriverTester() :
      LinuxGpioDriverTesterBase("Tester", MAX_HISTORY_SIZE),
      component("GP")
      ,m_cycles(0)
      ,m_currCycle(0)
  {
    this->initComponents();
    this->connectPorts();
  }

  LinuxGpioDriverTester ::
    ~LinuxGpioDriverTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTester ::
    testInterrupt(NATIVE_INT_TYPE gpio, NATIVE_INT_TYPE cycles)
  {
      // initialize the driver
      if (not this->component.open(gpio,LinuxGpioDriverComponentImpl::GPIO_INT)) {
          return;
      }
      this->m_cycles = cycles;
      this->m_currCycle = 0;
      // start the thread
      Os::Task::TaskStatus stat = this->component.startIntTask(99);
      if (stat != Os::Task::TASK_OK) {
          return;
      }

      // delay waiting for cycles to complete
      NATIVE_INT_TYPE maxCycles = 10;
      while (true) {
          Os::Task::delay(500);
          bool state;
          //this->invoke_to_gpioRead(0,state);
          printf("Wait %d %s\n",this->m_currCycle,state?"ON":"OFF");
          if ((this->m_currCycle >= this->m_cycles) or (maxCycles-- == 0)) {

#if 0
              printf("Interrupt cycle time diffs: \n");
              for (U32 i = 0; i < timerDiffIdx; ++i) {
                  printf("Diff number %d: %d\n",i,timerDiffList[i]);
              }
#endif

              this->component.exitThread();
              Os::Task::delay(500);
              return;
          }
      }

  }

  void LinuxGpioDriverTester ::
    testOutput(NATIVE_INT_TYPE gpio, NATIVE_INT_TYPE cycles) {

      this->component.open(gpio,LinuxGpioDriverComponentImpl::GPIO_OUT);

      bool state = true;

      for (NATIVE_INT_TYPE cycle = 0; cycle < cycles; cycle++) {
          printf("Cycle: %d\n",cycle);
          this->invoke_to_gpioWrite(0,state);
          if (state) {
              state = false;
          } else {
              state = true;
          }
          Os::Task::delay(500);
      }
  }

  void LinuxGpioDriverTester ::
    testInput(NATIVE_INT_TYPE gpio, NATIVE_INT_TYPE cycles) {

      this->component.open(gpio,LinuxGpioDriverComponentImpl::GPIO_IN);

      bool state = true;

      for (NATIVE_INT_TYPE cycle = 0; cycle < cycles; cycle++) {
          printf("Cycle: %d\n",cycle);
          this->invoke_to_gpioRead(0,state);
          if (state) {
              printf("1\n");
          } else {
              printf("0\n");
          }
          Os::Task::delay(500);
      }
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTester ::
    from_intOut_handler(
        const NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    timer.stop();
    U32 timeDiff = timer.getDiffUsec();
    timerDiffList[timerDiffIdx++] = timeDiff;
    timer.start();
    //printf("Cycle %d, time diff (usec): %d\n",this->m_currCycle,timeDiff);
    this->m_currCycle++;
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTester ::
    connectPorts()
  {

      // gpioRead
      this->connect_to_gpioRead(
          0,
          this->component.get_gpioRead_InputPort(0)
      );

      // gpioWrite
      this->connect_to_gpioWrite(
          0,
          this->component.get_gpioWrite_InputPort(0)
      );

      // intOut
      for (NATIVE_INT_TYPE i = 0; i < 2; ++i) {
        this->component.set_intOut_OutputPort(
            i,
            this->get_from_intOut(i)
        );
      }

      // Time
      this->component.set_Time_OutputPort(
          0,
          this->get_from_Time(0)
      );

      // Log
      this->component.set_Log_OutputPort(
          0,
          this->get_from_Log(0)
      );

      // LogText
      this->component.set_LogText_OutputPort(
          0,
          this->get_from_LogText(0)
      );

  }

  void LinuxGpioDriverTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

  void LinuxGpioDriverTester::textLogIn(
                   const FwEventIdType id, //!< The event ID
                   Fw::Time& timeTag, //!< The time
                   const Fw::TextLogSeverity severity, //!< The severity
                   const Fw::TextLogString& text //!< The event string
               ) {
       TextLogEntry e = { id, timeTag, severity, text };

       printTextLogHistoryEntry(e,stdout);
   }

} // end namespace Drv
