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
    : PassiveRateGroupComponentBase(compName),
      m_cycles(0),
      m_maxTime(0),
      m_portDurationHighWaterMarksUsec{},
      m_rawTimeSource(Os::RAWTIME_DEFAULT),
      m_numContexts(0) {}

PassiveRateGroup::~PassiveRateGroup() {}

Os::RawTime PassiveRateGroup::createRawTime() const {
    return Os::RawTime(this->m_rawTimeSource);
}

void PassiveRateGroup::configure(const ContextArray& contexts, const Os::RawTimeSource rawTimeSource) {
    static_assert(FW_NUM_ARRAY_ELEMENTS(m_contexts) == NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS,
                  "Context table size must match the number of rate group member output ports");

    this->m_numContexts = CONNECTION_COUNT_MAX;
    // copy context values
    for (FwIndexType entry = 0; entry < this->m_numContexts; entry++) {
        this->m_contexts[entry] = contexts[static_cast<FwSizeType>(entry)];
    }

    this->m_rawTimeSource = rawTimeSource;
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
    Os::RawTime endTime = this->createRawTime();
    FW_ASSERT(this->m_numContexts != 0);

    // Pre-allocate RawTime objects outside loop to avoid repeated constructor calls
    Os::RawTime portStart = this->createRawTime();
    Os::RawTime portEnd = this->createRawTime();
    PassiveRateGroup_CycleTime portTimes;

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
                portTimes[static_cast<FwSizeType>(port)] = cycleTime;
                // Update high water mark if current cycle time exceeds it
                if (cycleTime > this->m_portDurationHighWaterMarksUsec[static_cast<FwSizeType>(port)]) {
                    this->m_portDurationHighWaterMarksUsec[static_cast<FwSizeType>(port)] = cycleTime;
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

    // Lock mutex to protect statistics that can be cleared by CLEAR_STATISTICS command
    this->m_statisticsMutex.lock();

    // check to see if the time has exceeded the previous maximum
    if (cycleTime > this->m_maxTime) {
        this->m_maxTime = cycleTime;
    }

    U32 maxTime = this->m_maxTime;
    U32 cycles = ++this->m_cycles;
    PassiveRateGroup_CycleTime portDurationHighWaterMarks = this->m_portDurationHighWaterMarksUsec;

    this->m_statisticsMutex.unlock();

    if (Svc::PassiveRateGroupCfg::PortCycleTime) {
        this->tlmWrite_PortCycleTimeLast(portTimes);
        this->tlmWrite_PortCycleTimeHWM(portDurationHighWaterMarks);
    }

    this->tlmWrite_MaxCycleTime(maxTime);
    this->tlmWrite_CycleTime(cycleTime);
    this->tlmWrite_CycleCount(cycles);
}

void PassiveRateGroup::CLEAR_STATISTICS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Lock mutex to protect statistics shared with CycleIn handler
    this->m_statisticsMutex.lock();

    // Clear all port duration high water marks and max cycle time
    for (FwIndexType port = 0; port < NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; port++) {
        this->m_portDurationHighWaterMarksUsec[static_cast<FwSizeType>(port)] = 0;
    }
    this->m_maxTime = 0;
    // Note: m_cycles is intentionally NOT cleared - it's a running total

    this->m_statisticsMutex.unlock();

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
