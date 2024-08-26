
// ======================================================================
// \title  DeviceSm.cpp
// \author Auto-generated
// \brief  cpp file for state machine DeviceSm
//
// ======================================================================            
    
#include "stdio.h"
#include "assert.h"
#include "Fw/Types/SMSignalsSerializableAc.hpp"
#include "DeviceSm.hpp"


void FppTest::DeviceSm::init()
{
    this->state = OFF;

}


void FppTest::DeviceSm::update(const DeviceSmEvents signal, const Fw::SMSignalBuffer &data)
{
    switch (this->state) {
    
            /**
            * state OFF
            */
            case OFF:
            
            switch (signal) {

                case RTI_SIG:
                        this->state = ON;

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

                case RTI_SIG:
                        this->state = OFF;

                    break;
    
                default:
                    break;
            }
            break;
    
        default:
        assert(0);
    }
}
