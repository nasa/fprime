// ======================================================================
// \title  LinuxSerialDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxSerialDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Drv/LinuxSerialDriver/LinuxSerialDriverComponentImpl.hpp>

namespace Drv {

  LinuxSerialDriverComponentImpl ::
    ~LinuxSerialDriverComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LinuxSerialDriverComponentImpl ::
    serialSend_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer
    )
  {
    // TODO
  }

  void LinuxSerialDriverComponentImpl ::
    startReadThread(NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, NATIVE_INT_TYPE cpuAffinity) {
  }

  bool LinuxSerialDriverComponentImpl::open(const char* const device, UartBaudRate baud, UartFlowControl fc, UartParity parity, bool block) {

  }

  void LinuxSerialDriverComponentImpl ::
    quitReadThread(void) {
  }
} // end namespace Drv
