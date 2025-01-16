// ======================================================================
// \title  SmTest.cpp
// \author watney
// \brief  hpp file for SmTest component implementation class
// ======================================================================

#include <cstdio>

#include "FppTest/state_machine/external_instance/SmTest.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

SmTest::SmTest(const char* const compName): 
    SmTestComponentBase(compName) {}

SmTest ::~SmTest() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void SmTest::schedIn_handler(const NATIVE_INT_TYPE portNum, U32 context) {
    Fw::SmSignalBuffer data;

    device1_stateMachineInvoke(DeviceSm_Interface::DeviceSm_Signals::RTI_SIG, data);
    device2_stateMachineInvoke(DeviceSm_Interface::DeviceSm_Signals::RTI_SIG, data);
    device3_stateMachineInvoke(HackSm_Interface::HackSm_Signals::RTI_SIG, data);
    device4_stateMachineInvoke(HackSm_Interface::HackSm_Signals::RTI_SIG, data);
    device5_stateMachineInvoke(HackSm_Interface::HackSm_Signals::RTI_SIG, data);

}

//! Overflow hook for state machine device4
void SmTest::device4_stateMachineOverflowHook(
    const HackSm_Interface::HackSm_Signals signal, //!< The state machine signal
    const Fw::SmSignalBuffer& data //!< The state machine data
) {
    
}

void SmTest::DeviceSm_turnOn(const FwEnumStoreType stateMachineId) {
    printf("DeviceSm turnOn for state machine %d\n", stateMachineId);
}

void SmTest::DeviceSm_turnOff(const FwEnumStoreType stateMachineId) {
    printf("DeviceSm turnOff for state machine %d\n", stateMachineId);
}

void SmTest::DeviceSm_a1(
    const FwEnumStoreType stateMachineId, 
    const DeviceSm_Signals signal, 
    const Fw::SmSignalBuffer& data
    ) {
    printf("Action 1, stateMachineId = %d, signal = %d\n", stateMachineId, signal);
}


 bool SmTest::DeviceSm_g1(const FwEnumStoreType stateMachineId) {
    return true;
 }

 bool SmTest::DeviceSm_g2(
      const FwEnumStoreType stateMachineId, 
      const DeviceSm_Signals signal, 
      const Fw::SmSignalBuffer& data
    ) {
        return true;
    }

void SmTest::DeviceSm_a2(const FwEnumStoreType stateMachineId) {
    printf("Action 2\n");
}

void SmTest::HackSm_turnOn(const FwEnumStoreType stateMachineId) {
    printf("HackSm turn on\n");
}
    
void SmTest::HackSm_turnOff(const FwEnumStoreType stateMachineId) {
    printf("HackSm turn off\n");
}

void SmTest::HackSm_doDiag(const FwEnumStoreType stateMachineId) {
    printf("HackSm do diag\n");
}


// ----------------------------------------------------------------------
// Data product handler implementations
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

}  // end namespace FppTest
