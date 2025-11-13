// ======================================================================
// \title  SenderTester.cpp
// \author bocchino
// \brief  cpp file for Sender component test harness implementation class
// ======================================================================

#include "SenderTester.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SenderTester ::SenderTester() : SenderGTestBase("SenderTester", SenderTester::MAX_HISTORY_SIZE), component("Sender") {
    this->initComponents();
    this->connectPorts();
}

SenderTester ::~SenderTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SenderTester::sendData() {
    const U32 value = STest::Pick::any();
    this->component.sendData(value);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataOut(0, value);
}

}  // namespace FppTest
