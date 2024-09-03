// ======================================================================
// \title  Tester.cpp
// \author watney
// \brief  cpp file for SmTest test harness implementation class
// ======================================================================

#include <cstring>

#include "FppTest/state_machine/test/ut/Tester.hpp"
#include "Fw/Types/ExternalString.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester::Tester()
    : SmTestGTestBase("Tester", Tester::MAX_HISTORY_SIZE),
      component("SmTest") {
    this->initComponents();
    this->connectPorts();
    this->component.setIdBase(ID_BASE);
}

Tester::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester::schedIn_OK() {
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device1.state);
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device2.state);
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device3.state);
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device4.state);
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device5.state);
    invoke_to_schedIn(0,0);
    dispatchAll();
    ASSERT_EQ(DeviceSm::ON, this->component.m_stateMachine_device1.state);
    ASSERT_EQ(DeviceSm::ON, this->component.m_stateMachine_device2.state);
    ASSERT_EQ(DeviceSm::ON, this->component.m_stateMachine_device3.state);
    ASSERT_EQ(DeviceSm::ON, this->component.m_stateMachine_device4.state);
    ASSERT_EQ(DeviceSm::ON, this->component.m_stateMachine_device5.state);
    invoke_to_schedIn(0,0);
    dispatchAll();
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device1.state);
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device2.state);
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device3.state);
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device4.state);
    ASSERT_EQ(DeviceSm::OFF, this->component.m_stateMachine_device5.state);

    Fw::SMSignalBuffer data;
    this->component.device3_stateMachineInvoke(HackSm_Interface::HackSm_Signals::CHECK_SIG, data);
    dispatchAll();
    ASSERT_EQ(HackSm::DIAG, this->component.m_stateMachine_device3.state);
    invoke_to_schedIn(0,0);
    dispatchAll();
    ASSERT_EQ(HackSm::OFF, this->component.m_stateMachine_device3.state);

}


// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------
void Tester ::
    dispatchAll()
  {
    while (this->component.m_queue.getNumMsgs() > 0)
        this->component.doDispatch();
  }

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------


}  // end namespace FppTest
