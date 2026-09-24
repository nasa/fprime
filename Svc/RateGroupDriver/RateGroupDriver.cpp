#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/RateGroupDriver/RateGroupDriver.hpp>
#include <cstdio>
#include <cstring>
#include <limits>

namespace Svc {

RateGroupDriver::RateGroupDriver(const char* compName)
    : RateGroupDriverComponentBase(compName), m_ticks(0), m_rollover(1), m_configured(false) {}

FwSizeType RateGroupDriver::gcd(FwSizeType a, FwSizeType b) {
    while (b != 0) {
        FwSizeType temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

FwSizeType RateGroupDriver::lcm(FwSizeType a, FwSizeType b) {
    FW_ASSERT(a != 0);
    FW_ASSERT(b != 0);
    const FwSizeType g = gcd(a, b);
    FW_ASSERT(g != 0);
    const FwSizeType a_div_g = a / g;
    // Ensure that lcm will not overflow
    FW_ASSERT((std::numeric_limits<FwSizeType>::max() / b) >= a_div_g,
              static_cast<FwAssertArgType>(a),
              static_cast<FwAssertArgType>(b));
    return a_div_g * b;
}

void RateGroupDriver::configure(const DividerSet& dividerSet) {
    // verify port/table size matches
    static_assert(FW_NUM_ARRAY_ELEMENTS(m_dividers) == NUM_CYCLEOUT_OUTPUT_PORTS,
                  "Divider table size must match the number of cycle output ports");
    // reset rollover so a reconfigure does not carry over the prior rollover
    this->m_rollover = 1;
    // copy provided array of dividers
    for (FwIndexType entry = 0; entry < RateGroupDriver::DIVIDER_SIZE; entry++) {
        // A port with an offset equal or bigger than the divisor is not accepted because it would never be called
        FW_ASSERT((dividerSet.dividers[entry].offset == 0) ||
                      (dividerSet.dividers[entry].offset < dividerSet.dividers[entry].divisor),
                  static_cast<FwAssertArgType>(dividerSet.dividers[entry].offset),
                  static_cast<FwAssertArgType>(dividerSet.dividers[entry].divisor));
        this->m_dividers[entry] = dividerSet.dividers[entry];
        // rollover value should be the least common multiple of all dividers to make sure integer rollover doesn't jump cycles
        // only use non-zero dividers
        if (dividerSet.dividers[entry].divisor != 0) {
            this->m_rollover = RateGroupDriver::lcm(this->m_rollover, dividerSet.dividers[entry].divisor);
        }
    }
    this->m_configured = true;
}

RateGroupDriver::~RateGroupDriver() {}

void RateGroupDriver::CycleIn_handler(FwIndexType portNum, Os::RawTime& cycleStart) {
    // Make sure that the dividers have been configured:
    // If this asserts, add the configure() call to initialization.
    FW_ASSERT(this->m_configured);

    // Loop through each divider. For a given port, the port will be called when the divider value
    // divides evenly into the number of ticks. For example, if the divider value for a port is 4,
    // it would be called every fourth invocation of the CycleIn port.
    for (FwIndexType entry = 0; entry < RateGroupDriver::DIVIDER_SIZE; entry++) {
        if (this->m_dividers[entry].divisor != 0) {
            if (this->isConnected_CycleOut_OutputPort(static_cast<FwIndexType>(entry))) {
                if ((this->m_ticks % this->m_dividers[entry].divisor) == this->m_dividers[entry].offset) {
                    this->CycleOut_out(static_cast<FwIndexType>(entry), cycleStart);
                }
            }
        }
    }

    // rollover the tick value when the tick count reaches the rollover value
    // the rollover value is the least common multiple of all the dividers.
    FW_ASSERT(this->m_rollover > 0);
    this->m_ticks = (this->m_ticks + 1) % this->m_rollover;
}

}  // namespace Svc
