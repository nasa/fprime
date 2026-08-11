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
      m_portCycleTimeHWMUsec{},  // Zero-initialize atomic array
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
                // Update high water mark if current cycle time exceeds it (lock-free atomic)
                U32 currentHWM =
                    this->m_portCycleTimeHWMUsec[static_cast<FwSizeType>(port)].load(std::memory_order_relaxed);
                while (cycleTime > currentHWM) {
                    if (this->m_portCycleTimeHWMUsec[static_cast<FwSizeType>(port)].compare_exchange_weak(
                            currentHWM, cycleTime, std::memory_order_relaxed)) {
                        break;
                    }
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

    // Update max time atomically (lock-free, ISR-safe)
    U32 currentMax = this->m_maxTime.load(std::memory_order_relaxed);
    while (cycleTime > currentMax) {
        if (this->m_maxTime.compare_exchange_weak(currentMax, cycleTime, std::memory_order_relaxed)) {
            break;
        }
    }

    U32 maxTime = this->m_maxTime.load(std::memory_order_relaxed);
    U32 cycles = ++this->m_cycles;

    if (Svc::PassiveRateGroupCfg::PortCycleTime) {
        // Copy atomic array to telemetry structure for sending
        PassiveRateGroup_CycleTime portCycleTimeHWM;
        for (FwIndexType port = 0; port < this->getNum_RateGroupMemberOut_OutputPorts(); port++) {
            portCycleTimeHWM[static_cast<FwSizeType>(port)] =
                this->m_portCycleTimeHWMUsec[static_cast<FwSizeType>(port)].load(std::memory_order_relaxed);
        }
        this->tlmWrite_PortCycleTime(portTimes);
        this->tlmWrite_PortCycleTimeHWM(portCycleTimeHWM);
    }

    this->tlmWrite_MaxCycleTime(maxTime);
    this->tlmWrite_CycleTime(cycleTime);
    this->tlmWrite_CycleCount(cycles);
}

void PassiveRateGroup::CLEAR_STATISTICS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Clear max cycle time (lock-free atomic)
    this->m_maxTime.store(0, std::memory_order_relaxed);
    // Note: m_cycles is intentionally NOT cleared - it's a running total

    // Clear all port cycle time high water marks (lock-free atomic)
    for (FwIndexType port = 0; port < this->getNum_RateGroupMemberOut_OutputPorts(); port++) {
        this->m_portCycleTimeHWMUsec[static_cast<FwSizeType>(port)].store(0, std::memory_order_relaxed);
    }

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
