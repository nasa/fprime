// ======================================================================
// \title  ByteStreamBufferAdapterTester.cpp
// \author shahab
// \brief  cpp file for ByteStreamBufferAdapter component test harness implementation class
// ======================================================================

#include "ByteStreamBufferAdapterTester.hpp"

namespace Drv {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ByteStreamBufferAdapterTester ::ByteStreamBufferAdapterTester()
    : ByteStreamBufferAdapterGTestBase("ByteStreamBufferAdapterTester",
                                       ByteStreamBufferAdapterTester::MAX_HISTORY_SIZE),
      component("ByteStreamBufferAdapter") {
    this->initComponents();
    this->connectPorts();
}

ByteStreamBufferAdapterTester ::~ByteStreamBufferAdapterTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ByteStreamBufferAdapterTester ::toDo() {
    // TODO
}

}  // namespace Drv
