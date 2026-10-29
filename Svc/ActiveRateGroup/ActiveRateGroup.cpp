/*
 * \author: Tim Canham
 * \file:
 * \brief
 *
 * This file implements the ActiveRateGroup component,
 * which invokes a set of components the comprise the rate group.
 *
 *   Copyright 2014-2015, by the California Institute of Technology.
 *   ALL RIGHTS RESERVED. United States Government Sponsorship
 *   acknowledged.
 *
 */

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Console.hpp>
#include <Svc/ActiveRateGroup/ActiveRateGroup.hpp>
#include <config/ActiveRateGroupCfg.hpp>

namespace Svc {

// The cycle-started flag is stored from the cycle caller's thread, which may be interrupt context;
// a std::atomic implemented with a hidden lock would not be safe there.
static_assert(ATOMIC_BOOL_LOCK_FREE == 2, "std::atomic<bool> must be lock-free for the cycle-started flag");

ActiveRateGroup::ActiveRateGroup(const char* compName)
    : ActiveRateGroupComponentBase(compName),
      m_cycles(0),
      m_maxTime(0),
      m_cycleStarted(false),
      m_numContexts(0),
      m_overrunThrottle(0),
      m_cycleSlips(0) {}

void ActiveRateGroup::configure(const ContextArray& contexts) {
    static_assert(FW_NUM_ARRAY_ELEMENTS(m_contexts) == NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS,
                  "Context table size must match the number of rate group member output ports");

    this->m_numContexts = CONNECTION_COUNT_MAX;
    // copy context values
    for (FwIndexType entry = 0; entry < this->m_numContexts; entry++) {
        this->m_contexts[entry] = contexts[static_cast<FwSizeType>(entry)];
    }
}

void ActiveRateGroup::configure(const U32 contexts[], const FwIndexType numContexts) {
    FW_ASSERT(contexts != nullptr);
    FW_ASSERT(numContexts == this->getNum_RateGroupMemberOut_OutputPorts(), static_cast<FwAssertArgType>(numContexts),
              static_cast<FwAssertArgType>(this->getNum_RateGroupMemberOut_OutputPorts()));

    ContextArray contextArray;
    for (FwIndexType entry = 0; entry < numContexts; entry++) {
        contextArray[static_cast<FwSizeType>(entry)] = contexts[entry];
    }
    this->configure(contextArray);
}

ActiveRateGroup::~ActiveRateGroup() {}

void ActiveRateGroup::preamble() {
    this->log_DIAGNOSTIC_RateGroupStarted();
}

void ActiveRateGroup::CycleIn_handler(FwIndexType portNum, Os::RawTime& cycleStart) {
    // Make sure it's been configured
    FW_ASSERT(this->m_numContexts != 0);

    Os::RawTime endTime;

    // Clear the flag set by this cycle's CycleIn_preMsgHook. A cycle message whose hook ran
    // between the dequeue and this store is coalesced with this cycle: one slip per delay.
    this->m_cycleStarted.store(false, std::memory_order_relaxed);

    // invoke any members of the rate group
    for (FwIndexType port = 0; port < this->m_numContexts; port++) {
        if (this->isConnected_RateGroupMemberOut_OutputPort(port)) {
            this->RateGroupMemberOut_out(port, static_cast<U32>(this->m_contexts[port]));
        }
    }

    // grab timer for endTime of cycle
    Os::RawTime::Status timeStatus = endTime.now();
    if (timeStatus != Os::RawTime::Status::OP_OK) {
        this->log_WARNING_HI_RateGroupTimeGetError(Os::RawTimeStatus(static_cast<Os::RawTimeStatus::T>(timeStatus)));
    }

    // get rate group execution time
    U32 cycleTime;
    // Cast to void as the only possible error is overflow, which we can't handle other
    // than capping cycleTime to max value of U32 (which is done in getDiffUsec anyways)
    (void)endTime.getDiffUsec(cycleStart, cycleTime);

    // check to see if the time has exceeded the previous maximum
    if (cycleTime > this->m_maxTime) {
        this->m_maxTime = cycleTime;
    }

    // update cycle telemetry
    this->tlmWrite_RgMaxTime(this->m_maxTime);

    // check for cycle slip. That will happen if new cycle message has been received
    // which will cause flag will be set again.
    if (this->m_cycleStarted.load(std::memory_order_relaxed)) {
        this->m_cycleSlips++;
        if (this->m_overrunThrottle < ACTIVE_RATE_GROUP_OVERRUN_THROTTLE) {
            this->log_WARNING_HI_RateGroupCycleSlip(this->m_cycles);
            this->m_overrunThrottle++;
        }
        // update cycle slips
        this->tlmWrite_RgCycleSlips(this->m_cycleSlips);
    } else {  // if cycle is okay start decrementing throttle value
        if (this->m_overrunThrottle > 0) {
            this->m_overrunThrottle--;
        }
    }

    // increment cycle
    this->m_cycles++;
}

void ActiveRateGroup::CycleIn_preMsgHook(FwIndexType portNum, Os::RawTime& cycleStart) {
    // set flag to indicate cycle has started. Check in thread for overflow.
    this->m_cycleStarted.store(true, std::memory_order_relaxed);
}

void ActiveRateGroup::PingIn_handler(FwIndexType portNum, U32 key) {
    // return the key to health
    this->PingOut_out(0, key);
}

}  // namespace Svc
