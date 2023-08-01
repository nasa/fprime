// ======================================================================
// \title  Empty/test/ut/Tester.cpp
// \author tiffany
// \brief  cpp file for Empty test harness implementation class
// ======================================================================

#include "Tester.hpp"

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester() : EmptyGTestBase("Tester", Tester::MAX_HISTORY_SIZE), component("Empty") {
    this->initComponents();
    this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::test() {
    // Nothing else to test in an empty component
}
