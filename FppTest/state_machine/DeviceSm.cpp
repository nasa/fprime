
// ======================================================================
// \title  DeviceSm.cpp
// \author Auto-generated
// \brief  cpp file for state machine DeviceSm
//
// ======================================================================            
    
#include "stdio.h"
#include "assert.h"
#include "SMEvents.hpp"
#include "DeviceSm.h"


void FppTest::DeviceSm::init()
{
    this->state = OFF;

}


void FppTest::DeviceSm::update(const Svc::SMEvents *e)
{
    switch (this->state) {
    
            /**
            * state OFF
            */
            case OFF:
            
            switch (e->geteventSignal()) {

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
            
            switch (e->geteventSignal()) {

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
