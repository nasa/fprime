/*
 * \author: Tim Canham
 * \file:
 * \brief
 *
 * This file implements the PassiveRateGroup component,
 * which invokes a set of components the comprise the rate group.
 *
 *   Copyright 2014-2015, by the California Institute of Technology.
 *   ALL RIGHTS RESERVED. United States Government Sponsorship
 *   acknowledged.
 */

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Console.hpp>
#include <Svc/PassiveRateGroup/PassiveRateGroup.hpp>
#include <config/PassiveRateGroupCfg.hpp>

namespace Svc {
PassiveRateGroup::PassiveRateGroup(const char* compName)
    : PassiveRateGroupComponentBase(compName), m_cycles(0), m_maxTime(0), m_numContexts(0) {
#if PASSIVE_RATE_GROUP_ENABLE_COMPONENT_TIMING
    for (FwIndexType port = 0; port < NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; port++) {
        this->m_componentMaxTime[port] = 0;
    }
#endif
}

PassiveRateGroup::~PassiveRateGroup() {}

void PassiveRateGroup::configure(const U32 contexts[], const FwIndexType numContexts) {
    FW_ASSERT(contexts);
    FW_ASSERT(numContexts == this->getNum_RateGroupMemberOut_OutputPorts(), static_cast<FwAssertArgType>(numContexts),
              static_cast<FwAssertArgType>(this->getNum_RateGroupMemberOut_OutputPorts()));
    FW_ASSERT(FW_NUM_ARRAY_ELEMENTS(this->m_contexts) == this->getNum_RateGroupMemberOut_OutputPorts(),
              static_cast<FwAssertArgType>(FW_NUM_ARRAY_ELEMENTS(this->m_contexts)),
              static_cast<FwAssertArgType>(this->getNum_RateGroupMemberOut_OutputPorts()));

    this->m_numContexts = numContexts;
    // copy context values
    for (FwIndexType entry = 0; entry < this->m_numContexts; entry++) {
        this->m_contexts[entry] = static_cast<U32>(contexts[entry]);
    }
}

void PassiveRateGroup::CycleIn_handler(FwIndexType portNum, Os::RawTime& cycleStart) {
    Os::RawTime endTime;
    FW_ASSERT(this->m_numContexts);

    // invoke any members of the rate group
    for (FwIndexType port = 0; port < this->getNum_RateGroupMemberOut_OutputPorts(); port++) {
        if (this->isConnected_RateGroupMemberOut_OutputPort(port)) {
#if PASSIVE_RATE_GROUP_ENABLE_COMPONENT_TIMING
            Os::RawTime componentStartTime;
            componentStartTime.now();
            this->RateGroupMemberOut_out(port, this->m_contexts[port]);
            Os::RawTime componentEndTime;
            componentEndTime.now();
            U32 componentTime = 0;
            (void)componentEndTime.getDiffUsec(componentStartTime, componentTime);
            if (componentTime > this->m_componentMaxTime[port]) {
                this->m_componentMaxTime[port] = componentTime;
                this->log_ACTIVITY_HI_ComponentMaxTimeUpdated(componentTime);
            }
#else
            this->RateGroupMemberOut_out(port, this->m_contexts[port]);
#endif
        }
    }

    // grab timer for endTime of cycle
    endTime.now();

    // get rate group execution time
    U32 cycleTime;
    // Cast to void as the only possible error is overflow, which we can't handle other
    // than capping cycleTime to max value of U32 (which is done in getDiffUsec anyways)
    (void)endTime.getDiffUsec(cycleStart, cycleTime);
    // check to see if the time has exceeded the previous maximum
    if (cycleTime > this->m_maxTime) {
        this->m_maxTime = cycleTime;
    }
    this->tlmWrite_MaxCycleTime(this->m_maxTime);
    this->tlmWrite_CycleTime(cycleTime);
    this->tlmWrite_CycleCount(++this->m_cycles);
}

}  // namespace Svc
