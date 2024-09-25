
// ======================================================================
// \title  DeviceSm.cpp
// \author Auto-generated
// \brief  cpp file for state machine DeviceSm
//
// ======================================================================            
    
#include <Fw/Types/Assert.hpp>
#include "DeviceSm.hpp"


void FppTest::DeviceSm::init(const FwEnumStoreType stateMachineId)
{
    parent->DeviceSm_turnOff(stateMachineId);
    this->state = OFF;

}


void FppTest::DeviceSm::update(
    const FwEnumStoreType stateMachineId, 
    const DeviceSm_Interface::DeviceSm_Signals signal, 
    const Fw::SmSignalBuffer &data
)
{
    switch (this->state) {
    
            /**
            * state OFF
            */
            case OFF:
            
            switch (signal) {

                case DeviceSm_Interface::DeviceSm_Signals::RTI_SIG:
                        if ( parent->DeviceSm_g1(stateMachineId) ) {
                            parent->DeviceSm_a1(stateMachineId, signal, data);
                            parent->DeviceSm_turnOn(stateMachineId);
                            this->state = ON;
                        }

                    break;
    
                default:
                    break;
            }
            break;
    
            /**
            * state ON
            */
            case ON:
            
            switch (signal) {

                case DeviceSm_Interface::DeviceSm_Signals::RTI_SIG:
                        if (parent->DeviceSm_g2(stateMachineId, signal, data) ) {
                            parent->DeviceSm_a2(stateMachineId);
                            parent->DeviceSm_turnOff(stateMachineId);
                            this->state = OFF;
                        }

                    break;
    
                default:
                    break;
            }
            break;
    
        default:
        FW_ASSERT(0);
    }
}
