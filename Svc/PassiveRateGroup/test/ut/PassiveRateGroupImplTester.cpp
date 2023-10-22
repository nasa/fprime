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

#include <cstdio>
#include <cstring>
#include <unistd.h>

namespace Svc {

void PassiveRateGroupTester::init(NATIVE_INT_TYPE instance) {
    PassiveRateGroupGTestBase::init();
}

PassiveRateGroupTester::PassiveRateGroupTester(Svc::PassiveRateGroup& inst)
    : PassiveRateGroupGTestBase("testerbase", 100), m_impl(inst), m_callOrder(0) {
    this->clearPortCalls();
}

void PassiveRateGroupTester::clearPortCalls() {
    memset(this->m_callLog, 0, sizeof(this->m_callLog));
    this->m_callOrder = 0;
}

PassiveRateGroupTester::~PassiveRateGroupTester() {}

void PassiveRateGroupTester::from_RateGroupMemberOut_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
    ASSERT_TRUE(portNum < static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(m_impl.m_RateGroupMemberOut_OutputPort)));
    this->m_callLog[portNum].portCalled = true;
    this->m_callLog[portNum].contextVal = context;
    this->m_callLog[portNum].order = this->m_callOrder++;
    // Adding a small sleep to ensure that the cycle time is bigger than 0 us
    usleep(1);
}

void PassiveRateGroupTester::runNominal(NATIVE_INT_TYPE contexts[],
                                            NATIVE_UINT_TYPE numContexts,
                                            NATIVE_INT_TYPE instance) {
    TEST_CASE(101.1.1, "Run nominal rate group execution");

    // clear events
    this->clearTlm();

    Svc::TimerVal timer;
    timer.take();

    // clear port call log
    this->clearPortCalls();

    REQUIREMENT("FPRIME-PRG-001");
    // call active rate group with timer val
    this->invoke_to_CycleIn(0, timer);


    // check calls
    REQUIREMENT("FPRIME-PRG-002");
    for (NATIVE_UINT_TYPE portNum = 0;
         portNum < static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_RateGroupMemberOut_OutputPort)); portNum++) {
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

}  // namespace Svc
