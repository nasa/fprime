// ======================================================================
// \title  ApidManagerTester.cpp
// \author chammard
// \brief  cpp file for ApidManager component test harness implementation class
// ======================================================================

#include "ApidManagerTester.hpp"

namespace Svc {

namespace CCSDS {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ApidManagerTester ::ApidManagerTester()
    : ApidManagerGTestBase("ApidManagerTester", ApidManagerTester::MAX_HISTORY_SIZE), component("ApidManager") {
    this->initComponents();
    this->connectPorts();
}

ApidManagerTester ::~ApidManagerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ApidManagerTester ::toDo() {
    // TODO
}

bool ApidManagerTester::GetExistingSeqCount::precondition(const ApidManagerTester& state) {
    return true;
}

void ApidManagerTester::GetExistingSeqCount::action(ApidManagerTester& state) {
    ASSERT_TRUE(true);
}

}  // namespace CCSDS

}  // namespace Svc
