// ======================================================================
// \title  SmTestTester.cpp
// \author watney
// \brief  cpp file for SmTest test harness implementation class
// ======================================================================

#include <cstring>

#include "FppTest/state_machine/external_instance/test/ut/SmTestTester.hpp"
#include "Fw/Types/ExternalString.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SmTestTester::SmTestTester()
    : SmTestGTestBase("SmTestTester", SmTestTester::MAX_HISTORY_SIZE),
      component("SmTest") {
    this->initComponents();
    this->connectPorts();
    this->component.setIdBase(ID_BASE);
}

SmTestTester::~SmTestTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SmTestTester::schedIn_OK() {
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

    Fw::SmSignalBuffer data;
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
void SmTestTester ::
    dispatchAll()
  {
    while (this->component.m_queue.getMessagesAvailable() > 0)
        this->component.doDispatch();
  }

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------


}  // end namespace FppTest
