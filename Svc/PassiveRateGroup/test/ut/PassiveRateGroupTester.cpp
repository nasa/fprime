/*
 * \author Tim Canham
 * \file
 * \brief
 *
 * This file is the test component for the active rate group unit test.
 *
 * Code Generated Source Code Header
 *
 *   Copyright 2014-2015, by the California Institute of Technology.
 *   ALL RIGHTS RESERVED. United States Government Sponsorship
 *   acknowledged.
 */

#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>
#include <Svc/PassiveRateGroup/test/ut/PassiveRateGroupTester.hpp>
#include <config/PassiveRateGroupCfg.hpp>

#include <unistd.h>
#include <cstdio>
#include <cstring>

namespace Svc {

PassiveRateGroupTester::PassiveRateGroupTester(Svc::PassiveRateGroup& inst)
    : PassiveRateGroupGTestBase("testerbase", 1000), m_impl(inst), m_callOrder(0) {
    this->clearPortCalls();
}

void PassiveRateGroupTester::clearPortCalls() {
    memset(this->m_callLog, 0, sizeof(this->m_callLog));
    this->m_callOrder = 0;
}

PassiveRateGroupTester::~PassiveRateGroupTester() {}

void PassiveRateGroupTester::from_RateGroupMemberOut_handler(FwIndexType portNum, U32 context) {
    ASSERT_TRUE(portNum < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(m_impl.m_RateGroupMemberOut_OutputPort)));
    this->m_callLog[portNum].portCalled = true;
    this->m_callLog[portNum].contextVal = context;
    this->m_callLog[portNum].order = this->m_callOrder++;
    // Adding a small sleep to ensure that the cycle time is bigger than 0 us
    usleep(1);
}

void PassiveRateGroupTester::runNominal(U32 contexts[], FwIndexType numContexts, FwEnumStoreType instance) {
    TEST_CASE(101.1.1, "Run nominal rate group execution");

    // clear events
    this->clearTlm();

    Os::RawTime timestamp;
    timestamp.now();

    // clear port call log
    this->clearPortCalls();

    REQUIREMENT("FPRIME-PRG-001");
    // call active rate group with timestamp val
    this->invoke_to_CycleIn(0, timestamp);

    // check calls
    REQUIREMENT("FPRIME-PRG-002");
    for (FwIndexType portNum = 0;
         portNum < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_RateGroupMemberOut_OutputPort));
         portNum++) {
        ASSERT_TRUE(this->m_callLog[portNum].portCalled);
        ASSERT_EQ(this->m_callLog[portNum].contextVal, contexts[portNum]);
        ASSERT_EQ(this->m_callLog[portNum].order, portNum);
    }
    // Cycle times should be non-zero
    REQUIREMENT("FPRIME-PRG-003");
    ASSERT_TLM_MaxCycleTime_SIZE(1);
    ASSERT_TLM_CycleTime_SIZE(1);
    ASSERT_TLM_CycleCount_SIZE(1);
    ASSERT_GT(this->tlmHistory_MaxCycleTime->at(0).arg, 0);
    ASSERT_GT(this->tlmHistory_CycleTime->at(0).arg, 0);
    ASSERT_GT(this->tlmHistory_CycleCount->at(0).arg, 0);
}

void PassiveRateGroupTester::runPortCycleTimeTest() {
    TEST_CASE(101.2.1, "Test per-port cycle time tracking");

    REQUIREMENT("FPRIME-PRG-004");

    // Clear telemetry
    this->clearTlm();

    Os::RawTime timestamp;
    timestamp.now();

    // Clear port call log
    this->clearPortCalls();

    // Per-port telemetry only sent if PortCycleTime feature is enabled
    if (Svc::PassiveRateGroupCfg::PortCycleTime) {
        // Run first cycle - both channels should send (initial values)
        timestamp.now();
        this->invoke_to_CycleIn(0, timestamp);

        // Both PortCycleTime and PortCycleTimeHWM should be sent on first cycle
        ASSERT_TLM_PortCycleTime_SIZE(1);
        ASSERT_TLM_PortCycleTimeHWM_SIZE(1);

        // Verify HWM values match the cycle times (first cycle, so HWM == cycle time)
        PassiveRateGroup_CycleTime firstLast = this->tlmHistory_PortCycleTime->at(0).arg;
        PassiveRateGroup_CycleTime firstHWM = this->tlmHistory_PortCycleTimeHWM->at(0).arg;

        bool foundNonZero = false;
        for (FwSizeType portIdx = 0; portIdx < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; portIdx++) {
            // On first cycle, HWM should equal the cycle time
            ASSERT_EQ(firstHWM[portIdx], firstLast[portIdx]);
            if (firstHWM[portIdx] > 0) {
                foundNonZero = true;
            }
        }
        ASSERT_TRUE(foundNonZero);

        // Run second cycle with delay - PortCycleTime will be sent (no "update on change")
        // but HWM might not change if cycle times are similar
        usleep(100);  // Add delay to vary timing
        timestamp.now();
        this->invoke_to_CycleIn(0, timestamp);

        // PortCycleTime is always sent (no "update on change" semantics)
        ASSERT_TLM_PortCycleTime_SIZE(2);

        // PortCycleTimeHWM may or may not have changed (update on change semantics)
        // Could be 1 (no change) or 2 (some HWM increased)
        ASSERT_GE(this->tlmHistory_PortCycleTimeHWM->size(), 1);
        ASSERT_LE(this->tlmHistory_PortCycleTimeHWM->size(), 2);

        // Run third cycle - similar pattern
        timestamp.now();
        this->invoke_to_CycleIn(0, timestamp);

        ASSERT_TLM_PortCycleTime_SIZE(3);

        // HWM could be anywhere from 1 (never increased) to 3 (increased every time)
        ASSERT_GE(this->tlmHistory_PortCycleTimeHWM->size(), 1);
        ASSERT_LE(this->tlmHistory_PortCycleTimeHWM->size(), 3);

        // Verify HWM is actually the maximum: HWM >= all previous cycle times
        // Get the latest HWM values
        PassiveRateGroup_CycleTime latestHWM =
            this->tlmHistory_PortCycleTimeHWM->at(this->tlmHistory_PortCycleTimeHWM->size() - 1).arg;

        // Verify HWM >= each cycle time for all three cycles
        for (U32 cycleIdx = 0; cycleIdx < 3; cycleIdx++) {
            PassiveRateGroup_CycleTime cycleTime = this->tlmHistory_PortCycleTime->at(cycleIdx).arg;
            for (FwSizeType portIdx = 0; portIdx < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; portIdx++) {
                // HWM must be >= the cycle time from each cycle (it's the max across all cycles)
                ASSERT_GE(latestHWM[portIdx], cycleTime[portIdx])
                    << "HWM[" << portIdx << "]=" << latestHWM[portIdx] << " should be >= cycle[" << cycleIdx << "]["
                    << portIdx << "]=" << cycleTime[portIdx];
            }
        }

        // Also verify HWM >= the most recent cycle time
        PassiveRateGroup_CycleTime lastCycleTime = this->tlmHistory_PortCycleTime->at(2U).arg;
        for (FwSizeType portIdx = 0; portIdx < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; portIdx++) {
            ASSERT_GE(latestHWM[portIdx], lastCycleTime[portIdx])
                << "HWM must be >= most recent cycle time for port " << portIdx;
        }

        // Clear telemetry to test deduplication explicitly
        this->clearTlm();

        // Manually set all HWMs to a known high value to prevent natural increases
        for (FwSizeType port = 0; port < this->m_impl.getNum_RateGroupMemberOut_OutputPorts(); port++) {
            this->m_impl.m_portCycleTimeHWMUsec[port].store(999999, std::memory_order_relaxed);
        }

        // Run two more cycles - HWM should not increase (already at max)
        // so PortCycleTimeHWM should only be sent ONCE on the first cycle after clearing
        // (initial send), then deduplicated on the second cycle
        timestamp.now();
        this->invoke_to_CycleIn(0, timestamp);

        ASSERT_TLM_PortCycleTime_SIZE(1);     // New cycle times sent
        ASSERT_TLM_PortCycleTimeHWM_SIZE(1);  // HWM sent (first read after clear)

        // Verify HWM values are the max we set (999999)
        PassiveRateGroup_CycleTime dedupHWM = this->tlmHistory_PortCycleTimeHWM->at(0).arg;
        for (FwSizeType portIdx = 0; portIdx < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; portIdx++) {
            ASSERT_EQ(dedupHWM[portIdx], 999999U) << "HWM should be set to max value for port " << portIdx;
        }

        // Verify HWM >> actual cycle times (proving HWM is the stored max, not the current value)
        PassiveRateGroup_CycleTime cycleTime = this->tlmHistory_PortCycleTime->at(0).arg;
        for (FwSizeType portIdx = 0; portIdx < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; portIdx++) {
            ASSERT_GT(dedupHWM[portIdx], cycleTime[portIdx])
                << "HWM (999999) should be much greater than actual cycle time for port " << portIdx;
        }

        // Second cycle with HWM unchanged - HWM should be deduplicated
        timestamp.now();
        this->invoke_to_CycleIn(0, timestamp);

        ASSERT_TLM_PortCycleTime_SIZE(2);     // New cycle times sent
        ASSERT_TLM_PortCycleTimeHWM_SIZE(1);  // HWM NOT sent (unchanged, deduplicated)

    } else {
        // Feature disabled - verify per-port telemetry NOT sent
        timestamp.now();
        this->invoke_to_CycleIn(0, timestamp);
        ASSERT_TLM_PortCycleTime_SIZE(0);
        ASSERT_TLM_PortCycleTimeHWM_SIZE(0);
    }
}

void PassiveRateGroupTester::runClearStatisticsTest() {
    TEST_CASE(101.3.1, "Test CLEAR_STATISTICS command");

    REQUIREMENT("FPRIME-PRG-005");

    // Clear telemetry and command history
    this->clearTlm();
    this->clearHistory();

    Os::RawTime timestamp;
    timestamp.now();

    // Clear port call log
    this->clearPortCalls();

    // Run several cycles to accumulate statistics
    for (int cycle = 0; cycle < 5; cycle++) {
        timestamp.now();
        this->invoke_to_CycleIn(0, timestamp);
    }

    // Verify we have non-zero statistics
    ASSERT_GT(this->tlmHistory_MaxCycleTime->at(this->tlmHistory_MaxCycleTime->size() - 1).arg, 0);
    U32 cycleCountBeforeClear = this->tlmHistory_CycleCount->at(this->tlmHistory_CycleCount->size() - 1).arg;
    ASSERT_EQ(cycleCountBeforeClear, 5U);

    // Save the pre-clear HWM values if PortCycleTime is enabled
    PassiveRateGroup_CycleTime preClClearHWM;
    if (Svc::PassiveRateGroupCfg::PortCycleTime) {
        ASSERT_GT(this->tlmHistory_PortCycleTimeHWM->size(), 0);
        preClClearHWM = this->tlmHistory_PortCycleTimeHWM->at(this->tlmHistory_PortCycleTimeHWM->size() - 1).arg;

        // Verify at least one port has a non-zero HWM before clearing
        bool foundNonZero = false;
        for (FwSizeType portIdx = 0; portIdx < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; portIdx++) {
            if (preClClearHWM[portIdx] > 0) {
                foundNonZero = true;
                break;
            }
        }
        ASSERT_TRUE(foundNonZero) << "Should have non-zero HWM values before CLEAR_STATISTICS";
    }

    // Clear the telemetry history to see fresh values after command
    this->clearTlm();

    // Send CLEAR_STATISTICS command (sync command, executes immediately)
    this->sendCmd_CLEAR_STATISTICS(0, 0);

    // Verify command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, Svc::PassiveRateGroup::OPCODE_CLEAR_STATISTICS, 0, Fw::CmdResponse::OK);

    // Run one more cycle to generate fresh telemetry
    timestamp.now();
    this->invoke_to_CycleIn(0, timestamp);

    // Verify max cycle time was reset - may or may not emit depending on whether it differs from pre-clear max
    // MaxCycleTime has "update on change" semantics, so if post-clear cycle takes same time as pre-clear max,
    // no telemetry is emitted (intermittent flake). Only verify if history is non-empty.
    ASSERT_TLM_CycleTime_SIZE(1);
    if (this->tlmHistory_MaxCycleTime->size() > 0) {
        ASSERT_EQ(this->tlmHistory_MaxCycleTime->at(0).arg, this->tlmHistory_CycleTime->at(0).arg);
    }

    // Verify cycle count was NOT reset - it should be 6 (5 before + 1 after)
    ASSERT_TLM_CycleCount_SIZE(1);
    ASSERT_EQ(this->tlmHistory_CycleCount->at(0).arg, 6U);

    // Verify per-port HWMs were cleared (FPRIME-PRG-005 primary effect)
    if (Svc::PassiveRateGroupCfg::PortCycleTime) {
        // After clear, HWM should restart from zero, so first post-clear cycle should have HWM == cycle time
        ASSERT_TLM_PortCycleTime_SIZE(1);
        ASSERT_TLM_PortCycleTimeHWM_SIZE(1);

        PassiveRateGroup_CycleTime postClearLast = this->tlmHistory_PortCycleTime->at(0).arg;
        PassiveRateGroup_CycleTime postClearHWM = this->tlmHistory_PortCycleTimeHWM->at(0).arg;

        // After clearing HWM, the first cycle should have HWM == cycle time (HWM restarted from zero)
        // This is the key test: if HWM was cleared, the first post-clear cycle has HWM == cycle time
        // because the HWM started from zero and this is the first measurement.
        for (FwSizeType portIdx = 0; portIdx < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; portIdx++) {
            ASSERT_EQ(postClearHWM[portIdx], postClearLast[portIdx])
                << "After CLEAR_STATISTICS, HWM[" << portIdx << "] should equal cycle time (restarted from zero)";
        }
    }
}

void PassiveRateGroupTester::runRawTimeSourceTest() {
    TEST_CASE(101.4.1, "Test RawTimeSource configuration");

    // Clear telemetry
    this->clearTlm();

    // Verify RawTime preserves source through copy constructor and assignment
    Os::RawTime original(Os::RAWTIME_DEFAULT);
    ASSERT_EQ(original.getSource(), Os::RAWTIME_DEFAULT);

    // Test copy constructor preserves source
    Os::RawTime copy(original);
    ASSERT_EQ(copy.getSource(), Os::RAWTIME_DEFAULT);

    // Test assignment operator preserves source
    Os::RawTime assigned(Os::RAWTIME_DEFAULT);
    assigned = original;
    ASSERT_EQ(assigned.getSource(), Os::RAWTIME_DEFAULT);

    // Test that PassiveRateGroup's createRawTime uses the configured source
    Os::RawTime timestamp = this->m_impl.createRawTime();
    ASSERT_EQ(timestamp.getSource(), Os::RAWTIME_DEFAULT);

    timestamp.now();

    // Clear port call log
    this->clearPortCalls();

    // Invoke one cycle to verify the configured timer source works
    this->invoke_to_CycleIn(0, timestamp);

    // Verify telemetry was sent (proves timer functionality works with configured source)
    ASSERT_TLM_MaxCycleTime_SIZE(1);
    ASSERT_TLM_CycleTime_SIZE(1);
    ASSERT_TLM_CycleCount_SIZE(1);

    // Verify cycle time is non-zero (timer is actually measuring time)
    ASSERT_GT(this->tlmHistory_CycleTime->at(0).arg, 0U);
}

}  // namespace Svc
