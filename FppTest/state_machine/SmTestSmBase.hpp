
#ifndef SMTEST_SM_BASE_HPP
#define SMTEST_SM_BASE_HPP
// ======================================================================
// \title  SmTestSmBase.hpp
// \author Auto-generated
// \brief  Header file for the state machine base class
//
// ======================================================================            
#include "FppTest/state_machine/SmTestComponentAc.hpp"
#include "FppTest/state_machine/DeviceSm.h"
                                
namespace FppTest {
    namespace StateMachine {
        typedef enum {
            DEVICE1,
            DEVICE2,
        } SmId;                           
    };

    class SmTestSmBase : public SmTestComponentBase
        ,public DeviceSmIf
                                
    {
        public:
            SmTestSmBase(const char* const compName);
            void init(
                        NATIVE_INT_TYPE queueDepth,
                        NATIVE_INT_TYPE instance
            );
            
            // Interface to send an event to the state-machine
            void sendEvent(U32 eventSignal, StateMachine::SmId id);

            // Internal Interface handler for sendEvents
            void sendEvents_internalInterfaceHandler(const Svc::SMEvents& ev);
                                
            // Instantiate the state machines
            DeviceSm device1;
            DeviceSm device2;
            
                                
    };
}
#endif

            