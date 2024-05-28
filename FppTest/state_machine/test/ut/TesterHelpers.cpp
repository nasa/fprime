// ======================================================================
// \title  DpTest/test/ut/TesterHelpers.cpp
// \author Auto-generated
// \brief  cpp file for DpTest component test harness base class
//
// NOTE: this file was automatically generated
//
// ======================================================================
#include "Tester.hpp"

namespace FppTest {
// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts() {
   
    // schedIn
    this->connect_to_schedIn(0, this->component.get_schedIn_InputPort(0));
}

void Tester ::initComponents() {
    this->init();
    this->component.init(Tester::TEST_INSTANCE_QUEUE_DEPTH, Tester::TEST_INSTANCE_ID);
}

}  // end namespace FppTest
