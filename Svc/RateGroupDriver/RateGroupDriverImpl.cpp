#include <Svc/RateGroupDriver/RateGroupDriverImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <cstring>
#include <Fw/Types/Assert.hpp>
#include <stdio.h>

namespace Svc {

    RateGroupDriverImpl::RateGroupDriverImpl(const char* compName, NATIVE_INT_TYPE dividers[], NATIVE_INT_TYPE offsets[], NATIVE_INT_TYPE numDividers) :
        RateGroupDriverComponentBase(compName),
    m_ticks(0),m_rollover(1)
    {

        // double check arguments
        FW_ASSERT(dividers);
        FW_ASSERT(offsets);
        FW_ASSERT(numDividers <= static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)),
                numDividers,
                static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)));
        FW_ASSERT(numDividers <= static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_offsets)),
                  numDividers,
                  static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_offsets)));
        // verify port/table size matches
        FW_ASSERT(FW_NUM_ARRAY_ELEMENTS(this->m_dividers) == this->getNum_CycleOut_OutputPorts(),
                static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)),
                this->getNum_CycleOut_OutputPorts());
        // clear table
        ::memset(this->m_dividers,0,sizeof(this->m_dividers));
        ::memset(this->m_offsets,0,sizeof(this->m_offsets));
        for (NATIVE_INT_TYPE entry = 0; entry < numDividers; entry++) {
            this->m_dividers[entry] = dividers[entry];
            this->m_offsets[entry] = offsets[entry];
            // rollover value should be product of all dividers to make sure integer rollover doesn't jump cycles
            // only use non-zero dividers
            if (dividers[entry] != 0) {
                this->m_rollover *= dividers[entry];
            }
        }

    }

    RateGroupDriverImpl::~RateGroupDriverImpl(void) {

    }
    
    void RateGroupDriverImpl::init(void) {
        RateGroupDriverComponentBase::init();
    }

    void RateGroupDriverImpl::CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {

        // Loop through each divider. For a given port, the port will be called when the divider value
        // divides evenly into the number of ticks. For example, if the divider value for a port is 4,
        // it would be called every fourth invocation of the CycleIn port.
        for (NATIVE_INT_TYPE entry = 0; entry < static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)); entry++) {
            if (this->m_dividers[entry] != 0) {
                if (this->isConnected_CycleOut_OutputPort(entry)) {
                    if (((this->m_ticks + m_offsets[entry]) % this->m_dividers[entry]) == 0) {
                        this->CycleOut_out(entry,cycleStart);
                    }
                }
            }
        }
        
        // rollover the tick value when the tick count reaches the rollover value
        // the rollover value is the product of all the dividers. See comment in constructor.
        this->m_ticks = (this->m_ticks + 1) % this->m_rollover;

    }

}
