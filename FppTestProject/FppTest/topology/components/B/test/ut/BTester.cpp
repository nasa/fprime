// ======================================================================
// \title  BTester.cpp
// \author bocchino
// \brief  cpp file for B component test harness implementation class
// ======================================================================

#include "BTester.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

BTester::BTester() : BGTestBase("BTester", BTester::MAX_HISTORY_SIZE), component("B") {
    this->initComponents();
    this->connectPorts();
}

BTester::~BTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BTester::receiveData() {
    const U32 value = STest::Pick::any();
    this->invoke_to_dataIn(0, value);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataOut(0, value);
}

}  // namespace FppTest
