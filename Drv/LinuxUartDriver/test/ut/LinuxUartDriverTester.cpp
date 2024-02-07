// ======================================================================
// \title  LinuxUartDriverTester.cpp
// \author tcanham
// \brief  cpp file for LinuxUartDriver component test harness implementation class
// ======================================================================

#include "LinuxUartDriverTester.hpp"

namespace Drv {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

LinuxUartDriverTester ::LinuxUartDriverTester()
    : LinuxUartDriverGTestBase("LinuxUartDriverTester", LinuxUartDriverTester::MAX_HISTORY_SIZE),
      component("LinuxUartDriver") {
    this->initComponents();
    this->connectPorts();
}

LinuxUartDriverTester ::~LinuxUartDriverTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void LinuxUartDriverTester ::toDo() {
    // TODO
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

Fw::Buffer LinuxUartDriverTester ::from_allocate_handler(NATIVE_INT_TYPE portNum, U32 size) {
    return Fw::Buffer();
}

void LinuxUartDriverTester ::from_deallocate_handler(NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    // TODO
}

void LinuxUartDriverTester ::from_ready_handler(NATIVE_INT_TYPE portNum) {
    // TODO
}

void LinuxUartDriverTester ::from_recv_handler(NATIVE_INT_TYPE portNum,
                                               Fw::Buffer& recvBuffer,
                                               const Drv::RecvStatus& recvStatus) {
    // TODO
}

}  // namespace Drv
