// ======================================================================
// \title  LinuxUartDriverImpl.cpp
// \author tcanham
// \brief  cpp file for LinuxUartDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Drv/LinuxUartDriver/LinuxUartDriver.hpp>

namespace Drv {

LinuxUartDriver ::~LinuxUartDriver() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void LinuxUartDriver ::send_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& serBuffer) {
    // TODO
}

void LinuxUartDriver ::startReadThread(NATIVE_INT_TYPE priority,
                                       NATIVE_INT_TYPE stackSize,
                                       NATIVE_INT_TYPE cpuAffinity) {}

bool LinuxUartDriver::open(const char* const device,
                           UartBaudRate baud,
                           UartFlowControl fc,
                           UartParity parity,
                           bool block) {
    // TODO
    return false;
}

void LinuxUartDriver ::quitReadThread() {}
}  // end namespace Drv
