// ======================================================================
// \title  LinuxSerialDriver.hpp
// \author tcanham
// \brief  cpp file for LinuxSerialDriver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"
#include <stdlib.h>
#include <stdio.h>


#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester(const char* const device, NATIVE_INT_TYPE numReadBuffers, NATIVE_INT_TYPE bufferSize,
           LinuxSerialDriverComponentImpl::UartFlowControl flow,
           LinuxSerialDriverComponentImpl::UartParity parity) :
#if FW_OBJECT_NAMES == 1
    LinuxSerialDriverTesterBase("Tester", MAX_HISTORY_SIZE),
      component("LinuxSerialDriver")
#else
  LinuxSerialDriverTesterBase(MAX_HISTORY_SIZE),
      component()
#endif
     ,m_numBuffers(numReadBuffers),m_bufferSize(bufferSize)
  {
    this->initComponents();
    this->connectPorts();

    // allocate and configure buffers
    for (NATIVE_INT_TYPE buffer = 0; buffer < m_numBuffers; buffer++) {
        // initialize buffers
        this->m_readData[buffer] = (BYTE*)malloc(bufferSize);
        FW_ASSERT(this->m_readData[buffer]);
        this->m_readBuffer[buffer].setdata((U64)this->m_readData[buffer]);
        this->m_readBuffer[buffer].setsize(bufferSize);
        this->m_readBuffer[buffer].setbufferID(buffer);

        // push buffers to serial port
        this->invoke_to_readBufferSend(0,this->m_readBuffer[buffer]);
    }

    // configure port
    this->component.open(device,LinuxSerialDriverComponentImpl::BAUD_921K,flow,parity,true);

    // spawn driver thread
    this->component.startReadThread(90,20*1024);

  }

  Tester ::
    ~Tester(void)
  {
      // kill thread
      this->component.quitReadThread();
      // free buffers
      for (NATIVE_INT_TYPE buffer = 0; buffer < m_numBuffers; buffer++) {
          // initialize buffers
          free(this->m_readData[buffer]);
      }

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
      sendSerial(BYTE* data, NATIVE_INT_TYPE size)
  {
    Fw::Buffer buff;
    buff.setdata((U64)data);
    buff.setsize(size);

    this->invoke_to_serialSend(0,buff);

  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_serialRecv_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer,
        Drv::SerialReadStatus& status
    )
  {
//    this->pushFromPortEntry_serialRecv(serBuffer, status);

      static int numPkts = 0.0;
      static int numErrs = 0;
      //static BYTE expVal = 253; // ramp down, skipping xon and xoff
      //static BYTE expVal = 255; // ramp down
      static BYTE expVal = 0; // ramp up

      numPkts++;

      BYTE* ptr = (BYTE*) serBuffer.getdata();
      U32 error = 0;
      printf("<< size: %d\n",serBuffer.getsize());
      for (NATIVE_UINT_TYPE byte = 0; byte < serBuffer.getsize(); byte++) {
	//printf("%d: 0x%02X ",byte,ptr[byte]);

          // For this verification to work, must be sent ramping pattern of size of at least 256
#if 1
          // ramp down, skipped xon and xoff (17 and 19) when sending
          /*if (expVal == 19) expVal -= 1;
          if (expVal == 17) expVal -= 1;*/

          if (ptr[byte] != expVal && error == 0) {

	    //error = 1;
              printf("ERROR: exp: %d idx: %d got: %d size: %d\n",expVal,byte,ptr[byte],serBuffer.getsize());
          }
          //--expVal; // ramp down
          ++expVal; // ramp up
#endif
      }

      //expVal = ptr[serBuffer.getsize()-1]-1; // ramp down
      expVal = ptr[serBuffer.getsize()-1]+1; // ramp up

      if (error) {
          numErrs++;
          for (NATIVE_UINT_TYPE byte = 0; byte < serBuffer.getsize(); byte++) {
	    //printf("%d: 0x%02X ",byte,ptr[byte]);
          }

      }
      printf("\nTOTAL NUMBER OF ERRORS: %d\n",numErrs);
      printf("TOTAL ERROR PERCENTAGE: %f\n", ((double)numErrs/(double)numPkts)*100.0);
      printf("TOTAL NUMBER OF PACKETS: %d\n",numPkts);
      printf("\n");
      printf("\n");

      // send buffer back
      this->m_readBuffer[serBuffer.getbufferID()].setsize(this->m_bufferSize);
      this->invoke_to_readBufferSend(0,this->m_readBuffer[serBuffer.getbufferID()]);
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts(void)
  {

    // serialSend
    this->connect_to_serialSend(
        0,
        this->component.get_serialSend_InputPort(0)
    );

    // readBufferSend
    this->connect_to_readBufferSend(
        0,
        this->component.get_readBufferSend_InputPort(0)
    );

    // serialRecv
    this->component.set_serialRecv_OutputPort(
        0,
        this->get_from_serialRecv(0)
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

  void Tester ::
    initComponents(void)
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

  void Tester::textLogIn(
                   const FwEventIdType id, //!< The event ID
                   Fw::Time& timeTag, //!< The time
                   const Fw::TextLogSeverity severity, //!< The severity
                   const Fw::TextLogString& text //!< The event string
               ) {
       TextLogEntry e = { id, timeTag, severity, text };

       printTextLogHistoryEntry(e,stdout);
   }

} // end namespace Drv
