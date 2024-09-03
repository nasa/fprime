
// ======================================================================
// \title  HackSm.cpp
// \author Auto-generated
// \brief  cpp file for state machine HackSm
//
// ======================================================================            
    
#include <Fw/Types/Assert.hpp>
#include "HackSm.hpp"


void FppTest::HackSm::init(const FwEnumStoreType stateMachineId)
{
    parent->HackSm_turnOff(stateMachineId);
    this->state = OFF;

}


void FppTest::HackSm::update(
    const FwEnumStoreType stateMachineId, 
    const HackSm_Interface::HackSm_Signals signal, 
    const Fw::SmSignalBuffer &data
)
{
    switch (this->state) {
    
            /**
            * state OFF
            */
            case OFF:
            
            switch (signal) {

                case HackSm_Interface::HackSm_Signals::RTI_SIG:
                        parent->HackSm_turnOn(stateMachineId);
                        this->state = ON;

                    break;
    
                case HackSm_Interface::HackSm_Signals::CHECK_SIG:
                        parent->HackSm_doDiag(stateMachineId);
                        this->state = DIAG;

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

                case HackSm_Interface::HackSm_Signals::RTI_SIG:
                        parent->HackSm_turnOff(stateMachineId);
                        this->state = OFF;

                    break;
    
                case HackSm_Interface::HackSm_Signals::CHECK_SIG:
                        parent->HackSm_doDiag(stateMachineId);
                        this->state = DIAG;

                    break;
    
                default:
                    break;
            }
            break;
    
            /**
            * state DIAG
            */
            case DIAG:
            
            switch (signal) {

                case HackSm_Interface::HackSm_Signals::RTI_SIG:
                        parent->HackSm_turnOff(stateMachineId);
                        this->state = OFF;

                    break;
    
                default:
                    break;
            }
            break;
    
        default:
        FW_ASSERT(0);
    }
}
