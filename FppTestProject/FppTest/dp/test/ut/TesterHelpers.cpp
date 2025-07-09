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
    // productRecvIn
    this->connect_to_productRecvIn(0, this->component.get_productRecvIn_InputPort(0));

    // schedIn
    this->connect_to_schedIn(0, this->component.get_schedIn_InputPort(0));

    // productGetOut
    this->component.set_productGetOut_OutputPort(0, this->get_from_productGetOut(0));

    // productRequestOut
    this->component.set_productRequestOut_OutputPort(0, this->get_from_productRequestOut(0));

    // productSendOut
    this->component.set_productSendOut_OutputPort(0, this->get_from_productSendOut(0));

    // timeGetOut
    this->component.set_timeGetOut_OutputPort(0, this->get_from_timeGetOut(0));
}

void Tester ::initComponents() {
    this->init();
    this->component.init(Tester::TEST_INSTANCE_QUEUE_DEPTH, Tester::TEST_INSTANCE_ID);
}

}  // end namespace FppTest
