// ======================================================================
// \title  SmTest.cpp
// \author watney
// \brief  hpp file for SmTest component implementation class
// ======================================================================

#include <cstdio>

#include "FppTest/state_machine/SmTest.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

SmTest::SmTest(const char* const compName): 
    SmTestComponentBase(compName) {}

void SmTest ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    SmTestComponentBase::init(queueDepth, instance);
}

SmTest ::~SmTest() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void SmTest::schedIn_handler(const NATIVE_INT_TYPE portNum, U32 context) {
    Svc::SMEvents event;
    event.seteventSignal(DeviceSm::RTI_SIG);

    event.setsmId(StateMachine::DEVICE1);
    sendEvents_internalInterfaceInvoke(event);

    event.setsmId(StateMachine::DEVICE2);
    sendEvents_internalInterfaceInvoke(event);

}

// ----------------------------------------------------------------------
// Data product handler implementations
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

}  // end namespace FppTest
