
// ======================================================================
// \title  SmTestSmBase.cpp
// \author Auto-generated
// \brief  Cpp file for the state machine base class
//
// ======================================================================            
#include "FppTest/state_machine/SmTestSmBase.hpp"
#include "FppTest/state_machine/DeviceSm.h"
                                
FppTest::SmTestSmBase::SmTestSmBase(const char* const compName):
    SmTestComponentBase(compName)
    ,device1(this)
    ,device2(this)
{
                                
}                               

void FppTest::SmTestSmBase::init(
            NATIVE_INT_TYPE queueDepth,
            NATIVE_INT_TYPE instance)
{
    SmTestComponentBase::init(queueDepth, instance);
                                
    // Initialize the state machine
    device1.init();
    device2.init();
    
} 

void FppTest::SmTestSmBase:: sendEvent(U32 eventSignal, StateMachine::SmId id) {
                                
    Svc::SMEvents event;
    event.seteventSignal(eventSignal);
    event.setsmId(id);
    sendEvents_internalInterfaceInvoke(event);
}

void FppTest::SmTestSmBase::sendEvents_internalInterfaceHandler(const Svc::SMEvents& ev)
{
    U16 id = ev.getsmId();
    switch (id) {
                                
        case StateMachine::DEVICE1:
            this->device1.update(&ev);
            break;
        case StateMachine::DEVICE2:
            this->device2.update(&ev);
            break;
        default:
            FW_ASSERT(0);
    }

}
            