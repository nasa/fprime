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

#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Log.hpp>
#include <Svc/PassiveRateGroup/PassiveRateGroup.hpp>

namespace Svc {
PassiveRateGroup::PassiveRateGroup(const char* compName)
    : PassiveRateGroupComponentBase(compName), m_cycles(0), m_maxTime(0), m_numContexts(0) {
}

PassiveRateGroup::~PassiveRateGroup() {}

void PassiveRateGroup::configure(NATIVE_INT_TYPE contexts[], NATIVE_INT_TYPE numContexts) {
    FW_ASSERT(contexts);
    FW_ASSERT(numContexts == this->getNum_RateGroupMemberOut_OutputPorts(),numContexts,this->getNum_RateGroupMemberOut_OutputPorts());
    FW_ASSERT(FW_NUM_ARRAY_ELEMENTS(this->m_contexts) == this->getNum_RateGroupMemberOut_OutputPorts(),
              FW_NUM_ARRAY_ELEMENTS(this->m_contexts),
              this->getNum_RateGroupMemberOut_OutputPorts());

    this->m_numContexts = numContexts;
    // copy context values
    for (NATIVE_INT_TYPE entry = 0; entry < this->m_numContexts; entry++) {
        this->m_contexts[entry] = static_cast<U32>(contexts[entry]);
    }
}


void PassiveRateGroup::CycleIn_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {
    TimerVal end;
    FW_ASSERT(this->m_numContexts);

    // invoke any members of the rate group
    for (NATIVE_INT_TYPE port = 0; port < this->getNum_RateGroupMemberOut_OutputPorts(); port++) {
        if (this->isConnected_RateGroupMemberOut_OutputPort(port)) {
            this->RateGroupMemberOut_out(port, this->m_contexts[port]);
        }
    }

    // grab timer for end of cycle
    end.take();

    // get rate group execution time
    U32 cycle_time = end.diffUSec(cycleStart);

    // check to see if the time has exceeded the previous maximum
    if (cycle_time > this->m_maxTime) {
        this->m_maxTime = cycle_time;
    }
    this->tlmWrite_MaxCycleTime(this->m_maxTime);
    this->tlmWrite_CycleTime(cycle_time);
    this->tlmWrite_CycleCount(++this->m_cycles);
}

}  // namespace Svc
