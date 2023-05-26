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
*   acknowledged. Any commercial use must be negotiated with the Office
*   of Technology Transfer at the California Institute of Technology.
*
*   Information included herein is controlled under the International
*   Traffic in Arms Regulations ("ITAR") by the U.S. Department of State.
*   Export or transfer of this information to a Foreign Person or foreign
*   entity requires an export license issued by the U.S. State Department
*   or an ITAR exemption prior to the export or transfer.
*/

#include <Svc/PassiveRateGroup/test/ut/PassiveRateGroupImplTester.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>

#include <cstdio>
#include <cstring>

namespace Svc {

    void PassiveRateGroupImplTester::init(NATIVE_INT_TYPE instance) {
        PassiveRateGroupGTestBase::init();
    }

    PassiveRateGroupImplTester::PassiveRateGroupImplTester(Svc::PassiveRateGroupImpl& inst) :
            PassiveRateGroupGTestBase("testerbase",100),
            m_impl(inst),m_causeOverrun(false),m_callOrder(0) {
        this->clearPortCalls();
    }

    void PassiveRateGroupImplTester::clearPortCalls(void) {
        memset(this->m_callLog,0,sizeof(this->m_callLog));
        this->m_callOrder = 0;
    }


    PassiveRateGroupImplTester::~PassiveRateGroupImplTester() {
    }

    void PassiveRateGroupImplTester::from_RateGroupMemberOut_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
        ASSERT_TRUE(portNum < (NATIVE_INT_TYPE)FW_NUM_ARRAY_ELEMENTS(m_impl.m_RateGroupMemberOut_OutputPort));
        this->m_callLog[portNum].portCalled = true;
        this->m_callLog[portNum].contextVal = context;
        this->m_callLog[portNum].order = this->m_callOrder++;

    }

    void PassiveRateGroupImplTester::runNominal(NATIVE_UINT_TYPE contexts[], NATIVE_UINT_TYPE numContexts, NATIVE_INT_TYPE instance) {

        TEST_CASE(101.1.1,"Run nominal rate group execution");

        // clear events
        this->clearEvents();
        this->clearTlm();
        // call the preamble
        this->m_impl.preamble();
        // verify "task started" event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_PassiveRateGroupStarted_SIZE(1);

        Svc::TimerVal timer;
        timer.take();

        // clear port call log
        this->clearPortCalls();

        // call active rate group with timer val
        this->invoke_to_CycleIn(0,timer);

        REQUIREMENT("ISF-ARG-001");

        // check calls
        REQUIREMENT("ISF-ARG-002");
        for (NATIVE_UINT_TYPE portNum = 0; portNum <
        (NATIVE_INT_TYPE)FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_RateGroupMemberOut_OutputPort); portNum++) {
            ASSERT_TRUE(this->m_callLog[portNum].portCalled);
            ASSERT_EQ(this->m_callLog[portNum].contextVal,contexts[portNum]);
            ASSERT_EQ(this->m_callLog[portNum].order,portNum);
        }
        // Timer should be non-zero
        REQUIREMENT("ISF-ARG-003");

        // Should have gotten write of size
        ASSERT_TLM_SIZE(1);

    }

} /* namespace SvcTest */
