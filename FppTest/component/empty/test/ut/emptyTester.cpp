// ======================================================================
// \title  Empty/test/ut/Tester.cpp
// \author tiffany
// \brief  cpp file for Empty test harness implementation class
// ======================================================================

#include "emptyTester.hpp"

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

emptyTester ::emptyTester() : EmptyGTestBase("Tester", emptyTester::MAX_HISTORY_SIZE), component("Empty") {
    this->initComponents();
    this->connectPorts();
}

emptyTester ::~emptyTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void emptyTester ::test() {
    // Nothing else to test in an empty component
}
