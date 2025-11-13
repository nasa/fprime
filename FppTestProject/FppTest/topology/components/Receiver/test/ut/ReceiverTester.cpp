// ======================================================================
// \title  ReceiverTester.cpp
// \author bocchino
// \brief  cpp file for Receiver component test harness implementation class
// ======================================================================

#include "ReceiverTester.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ReceiverTester::ReceiverTester() : ReceiverGTestBase("ReceiverTester", ReceiverTester::MAX_HISTORY_SIZE), component("Receiver") {
    this->initComponents();
    this->connectPorts();
}

ReceiverTester::~ReceiverTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ReceiverTester::receiveData() {
    const U32 value = STest::Pick::any();
    this->invoke_to_dataIn(0, value);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataOut(0, value);
}

}  // namespace FppTest
