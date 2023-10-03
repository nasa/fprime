// ======================================================================
// \title  LinuxSpiDriver.hpp
// \author tcanham
// \brief  cpp file for LinuxSpiDriver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "LinuxSpiDriverTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  LinuxSpiDriverTester ::
    LinuxSpiDriverTester() :
    LinuxSpiDriverTesterBase("Tester", MAX_HISTORY_SIZE),
      component("LinuxSpiDriver")
  {
    this->initComponents();
    this->connectPorts();
  }

  LinuxSpiDriverTester ::
    ~LinuxSpiDriverTester()
  {

  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTester ::
    connectPorts()
  {

    // SpiReadWrite
    this->connect_to_SpiReadWrite(
        0,
        this->component.get_SpiReadWrite_InputPort(0)
    );

    // Tlm
    this->component.set_Tlm_OutputPort(
        0,
        this->get_from_Tlm(0)
    );

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

  void LinuxSpiDriverTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );

    this->component.open(8,0,SPI_FREQUENCY_1MHZ);
  }

  void LinuxSpiDriverTester::textLogIn(const FwEventIdType id, //!< The event ID
          Fw::Time& timeTag, //!< The time
          const Fw::TextLogSeverity severity, //!< The severity
          const Fw::TextLogString& text //!< The event string
          ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e, stdout);
  }

  void LinuxSpiDriverTester::sendBuffer(BYTE* buffer, NATIVE_INT_TYPE size) {
      Fw::Buffer w;
      w.setdata(reinterpret_cast<POINTER_CAST>(buffer));
      w.setsize(size);

      printf("WRITE: ");
      for (NATIVE_INT_TYPE byte = 0; byte < size; byte++) {
          printf("0x%02X ",buffer[byte]);
      }
      printf("\n");

      BYTE* rb = 0;
      rb = new BYTE[size];

      FW_ASSERT(rb);

      Fw::Buffer r(0,0, reinterpret_cast<POINTER_CAST>(rb),size);

      this->invoke_to_SpiReadWrite(0,w,r);

      BYTE* d = (BYTE*)r.getdata();
      printf("READ: ");
      for (NATIVE_INT_TYPE byte = 0; byte < size; byte++) {
          printf("0x%02X ",d[byte]);
      }
      printf("\n");

      delete[] rb;
  }

} // end namespace Drv
