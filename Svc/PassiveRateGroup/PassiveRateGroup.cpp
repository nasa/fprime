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
#include "config/FwSizeTypeAliasAc.h"

namespace Svc {
PassiveRateGroup::PassiveRateGroup(const char* compName)
    : PassiveRateGroupComponentBase(compName), m_cycles(0), m_maxTime(0), m_numContexts(0) {
    // Initialize port duration high water marks to zero
    for (FwIndexType port = 0; port < NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; port++) {
        this->m_portDurationHighWaterMarks[port] = 0;
    }
}

PassiveRateGroup::~PassiveRateGroup() {}

void PassiveRateGroup::configure(const ContextArray& contexts) {
    static_assert(FW_NUM_ARRAY_ELEMENTS(m_contexts) == NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS,
                  "Context table size must match the number of rate group member output ports");

    this->m_numContexts = CONNECTION_COUNT_MAX;
    // copy context values
    for (FwIndexType entry = 0; entry < this->m_numContexts; entry++) {
        this->m_contexts[entry] = contexts[static_cast<FwSizeType>(entry)];
    }
}

void PassiveRateGroup::configure(const U32 contexts[], const FwIndexType numContexts) {
    FW_ASSERT(contexts != nullptr);
    FW_ASSERT(numContexts == this->getNum_RateGroupMemberOut_OutputPorts(), static_cast<FwAssertArgType>(numContexts),
              static_cast<FwAssertArgType>(this->getNum_RateGroupMemberOut_OutputPorts()));

    ContextArray contextArray;
    for (FwIndexType entry = 0; entry < numContexts; entry++) {
        contextArray[static_cast<FwSizeType>(entry)] = static_cast<U32>(contexts[entry]);
    }
    this->configure(contextArray);
}

void PassiveRateGroup::CycleIn_handler(FwIndexType portNum, Os::RawTime& cycleStart) {
    Os::RawTime endTime;
    FW_ASSERT(this->m_numContexts != 0);

    // Pre-allocate RawTime objects outside loop to avoid repeated constructor calls
    Os::RawTime portStart;
    Os::RawTime portEnd;

    // invoke any members of the rate group
    for (FwIndexType port = 0; port < this->getNum_RateGroupMemberOut_OutputPorts(); port++) {
        if (this->isConnected_RateGroupMemberOut_OutputPort(port)) {
            if (Svc::PassiveRateGroupCfg::PortCycleTime) {
                (void)portStart.now();
            }

            this->RateGroupMemberOut_out(port, this->m_contexts[port]);

            if (Svc::PassiveRateGroupCfg::PortCycleTime) {
                (void)portEnd.now();
                U32 cycleTime;
                (void)portEnd.getDiffUsec(portStart, cycleTime);
                // Update high water mark if current cycle time exceeds it
                if (cycleTime > this->m_portDurationHighWaterMarks[port]) {
                    this->m_portDurationHighWaterMarks[port] = cycleTime;
                }
            }
        }
    }

    // grab timer for endTime of cycle
    (void)endTime.now();

    // get rate group execution time
    U32 cycleTime;
    // Cast to void as the only possible error is overflow, which we can't handle other
    // than capping cycleTime to max value of U32 (which is done in getDiffUsec anyways)
    (void)endTime.getDiffUsec(cycleStart, cycleTime);
    // check to see if the time has exceeded the previous maximum
    if (cycleTime > this->m_maxTime) {
        this->m_maxTime = cycleTime;
    }

    if (Svc::PassiveRateGroupCfg::PortCycleTime) {
        // Create array from HWM data for telemetry
        PassiveRateGroup_CycleTime hwmArray;
        for (FwSizeType i = 0; i < NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; i++) {
            hwmArray[i] = this->m_portDurationHighWaterMarks[i];
        }
        this->tlmWrite_PortCycleTime(hwmArray);
    }

    this->tlmWrite_MaxCycleTime(this->m_maxTime);
    this->tlmWrite_CycleTime(cycleTime);
    this->tlmWrite_CycleCount(++this->m_cycles);
}

void PassiveRateGroup::CLEAR_PORT_HWM_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Clear all port duration high water marks
    for (FwIndexType port = 0; port < NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; port++) {
        this->m_portDurationHighWaterMarks[port] = 0;
    }
    this->m_maxTime = 0;
    this->m_cycles = 0;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
