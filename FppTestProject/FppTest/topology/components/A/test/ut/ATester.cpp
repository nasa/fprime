// ======================================================================
// \title  ATester.cpp
// \author bocchino
// \brief  cpp file for A component test harness implementation class
// ======================================================================

#include "ATester.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ATester ::ATester() : AGTestBase("ATester", ATester::MAX_HISTORY_SIZE), component("A") {
    this->initComponents();
    this->connectPorts();
}

ATester ::~ATester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ATester::sendData() {
    const U32 value = STest::Pick::any();
    this->component.sendData(value);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataOut(0, value);
}

}  // namespace FppTest
