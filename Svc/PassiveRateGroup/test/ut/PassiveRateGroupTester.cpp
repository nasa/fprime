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
    : PassiveRateGroupGTestBase("testerbase", 100), m_impl(inst), m_callOrder(0) {
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

    // Run multiple cycles to build up high water marks
    for (int cycle = 0; cycle < 3; cycle++) {
        timestamp.now();
        this->invoke_to_CycleIn(0, timestamp);
    }

    // Per-port telemetry only sent if PortCycleTime feature is enabled
    if (Svc::PassiveRateGroupCfg::PortCycleTime) {
        // Verify per-port telemetry is sent
        ASSERT_TLM_PortCycleTimeLast_SIZE(3);
        ASSERT_TLM_PortCycleTimeHWM_SIZE(3);

        // Verify that port cycle times are non-zero (ports did work)
        for (int tlmIdx = 0; tlmIdx < 3; tlmIdx++) {
            bool foundNonZero = false;
            for (FwSizeType portIdx = 0; portIdx < Svc::PassiveRateGroup::CONNECTION_COUNT_MAX; portIdx++) {
                if (this->tlmHistory_PortCycleTimeLast->at(tlmIdx).arg[portIdx] > 0) {
                    foundNonZero = true;
                    break;
                }
            }
            ASSERT_TRUE(foundNonZero);
        }
    } else {
        // Feature disabled - verify per-port telemetry NOT sent
        ASSERT_TLM_PortCycleTimeLast_SIZE(0);
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
    ASSERT_EQ(cycleCountBeforeClear, 5);

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

    // Verify max cycle time was reset to current cycle time (not zero, since we just ran a cycle)
    ASSERT_TLM_MaxCycleTime_SIZE(1);
    ASSERT_TLM_CycleTime_SIZE(1);
    ASSERT_EQ(this->tlmHistory_MaxCycleTime->at(0).arg, this->tlmHistory_CycleTime->at(0).arg);

    // Verify cycle count was NOT reset - it should be 6 (5 before + 1 after)
    ASSERT_TLM_CycleCount_SIZE(1);
    ASSERT_EQ(this->tlmHistory_CycleCount->at(0).arg, 6);
}

}  // namespace Svc
