#include <Svc/RateGroupDriver/RateGroupDriver.hpp>
#include <FpConfig.hpp>
#include <cstring>
#include <Fw/Types/Assert.hpp>
#include <cstdio>

namespace Svc {

    RateGroupDriver::RateGroupDriver(const char* compName) :
        RateGroupDriverComponentBase(compName),
    m_numDividers(0),m_ticks(0),m_rollover(1) {

    }

    void RateGroupDriver::configure(DividerSet dividersSet)
    {

        // check arguments
        FW_ASSERT(dividersSet.dividers);
        this->m_numDividers = RateGroupDriver::DIVIDER_SIZE;
        FW_ASSERT(this->m_numDividers <= static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)),
                this->m_numDividers,
                static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)));
        // verify port/table size matches
        FW_ASSERT(FW_NUM_ARRAY_ELEMENTS(this->m_dividers) == this->getNum_CycleOut_OutputPorts(),
                static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_dividers)),
                this->getNum_CycleOut_OutputPorts());
        // copy provided array of dividers
        for (NATIVE_UINT_TYPE entry = 0; entry < RateGroupDriver::DIVIDER_SIZE; entry++) {
            // A port with an offset equal or bigger than the divisor is not accepted because it would never be called
            FW_ASSERT((this->m_dividers[entry].offset==0)||(this->m_dividers[entry].offset < this->m_dividers[entry].divisor),
                this->m_dividers[entry].offset,
                this->m_dividers[entry].divisor);
            this->m_dividers[entry] = dividersSet.dividers[entry];
            // rollover value should be product of all dividers to make sure integer rollover doesn't jump cycles
            // only use non-zero dividers
            if (dividersSet.dividers[entry].divisor != 0) {
                this->m_rollover *= dividersSet.dividers[entry].divisor;
            }
        }

    }

    RateGroupDriver::~RateGroupDriver() {

    }

    void RateGroupDriver::CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {

        // Make sure that the dividers have been configured:
        // If this asserts, add the configure() call to initialization.
        FW_ASSERT(this->m_numDividers);

        // Loop through each divider. For a given port, the port will be called when the divider value
        // divides evenly into the number of ticks. For example, if the divider value for a port is 4,
        // it would be called every fourth invocation of the CycleIn port.
        for (NATIVE_INT_TYPE entry = 0; entry < this->m_numDividers; entry++) {
            if (this->m_dividers[entry].divisor != 0) {
                if (this->isConnected_CycleOut_OutputPort(entry)) {
                    if ((this->m_ticks % this->m_dividers[entry].divisor) == this->m_dividers[entry].offset) {
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
