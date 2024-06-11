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
*
*/

#include <Svc/ActiveRateGroup/test/ut/ActiveRateGroupImplTester.hpp>
#include <ActiveRateGroupCfg.hpp>
#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>

#include <cstdio>
#include <cstring>

namespace Svc {

    void ActiveRateGroupImplTester::init(NATIVE_INT_TYPE instance) {
        ActiveRateGroupGTestBase::init();
    }

    ActiveRateGroupImplTester::ActiveRateGroupImplTester(Svc::ActiveRateGroup& inst) :
            ActiveRateGroupGTestBase("testerbase",100),
            m_impl(inst),m_causeOverrun(false),m_callOrder(0) {
        this->clearPortCalls();
    }

    void ActiveRateGroupImplTester::clearPortCalls() {
        memset(this->m_callLog,0,sizeof(this->m_callLog));
        this->m_callOrder = 0;
    }


    ActiveRateGroupImplTester::~ActiveRateGroupImplTester() {
    }

    void ActiveRateGroupImplTester::from_RateGroupMemberOut_handler(NATIVE_INT_TYPE portNum, U32 context) {
        ASSERT_TRUE(portNum < static_cast<NATIVE_INT_TYPE>(FW_NUM_ARRAY_ELEMENTS(m_impl.m_RateGroupMemberOut_OutputPort)));
        this->m_callLog[portNum].portCalled = true;
        this->m_callLog[portNum].contextVal = context;
        this->m_callLog[portNum].order = this->m_callOrder++;
        // we can cause an overrun by calling the cycle port in the middle of the rate
        // group execution
        if (this->m_causeOverrun) {
            TimerVal zero(0,0);
            this->invoke_to_CycleIn(0,zero);
            this->m_causeOverrun = false;
        }
    }

    void ActiveRateGroupImplTester ::
      from_PingOut_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
      this->pushFromPortEntry_PingOut(key);
    }

    void ActiveRateGroupImplTester::runNominal(NATIVE_INT_TYPE contexts[], NATIVE_INT_TYPE numContexts, NATIVE_INT_TYPE instance) {

        TEST_CASE(101.1.1,"Run nominal rate group execution");

        // clear events
        this->clearEvents();
        this->clearTlm();
        // call the preamble
        this->m_impl.preamble();
        // verify "task started" event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_RateGroupStarted_SIZE(1);

        Svc::TimerVal timer;
        timer.take();

        // clear port call log
        this->clearPortCalls();
        // verify cycle start flag is NOT set
        ASSERT_FALSE(this->m_impl.m_cycleStarted);
        // call active rate group with timer val
        this->invoke_to_CycleIn(0,timer);
        // verify cycle started flag is set
        ASSERT_TRUE(this->m_impl.m_cycleStarted);
        // call doDispatch() for ActiveRateGroup
        REQUIREMENT("ARG-001");
        this->m_impl.doDispatch();
        // verify cycle started flag is reset
        ASSERT_FALSE(this->m_impl.m_cycleStarted);
        // check calls
        REQUIREMENT("ARG-002");
        for (NATIVE_UINT_TYPE portNum = 0; portNum <
        static_cast<NATIVE_UINT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_RateGroupMemberOut_OutputPort)); portNum++) {
            ASSERT_TRUE(this->m_callLog[portNum].portCalled);
            ASSERT_EQ(this->m_callLog[portNum].contextVal,contexts[portNum]);
            ASSERT_EQ(this->m_callLog[portNum].order,portNum);
        }
        // Timer should be non-zero
        REQUIREMENT("ARG-003");

        // Should have gotten write of size
        ASSERT_TLM_SIZE(1);
        // Should not have slip
        ASSERT_EVENTS_RateGroupCycleSlip_SIZE(0);
        // Should not have increased cycle slip counter
        ASSERT_TLM_RgCycleSlips_SIZE(0);

    }

    void ActiveRateGroupImplTester::runCycleOverrun(NATIVE_INT_TYPE contexts[], NATIVE_INT_TYPE numContexts, NATIVE_INT_TYPE instance) {

        TEST_CASE(101.2.1,"Run cycle slip scenario");
        // call the preamble
        this->m_impl.preamble();
        // verify "task started" event
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_RateGroupStarted_SIZE(1);

        Svc::TimerVal timer(1,2);

        // run some more cycles to verify that event is sent and telemetry is updated
        for (NATIVE_INT_TYPE cycle = 0; cycle < ACTIVE_RATE_GROUP_OVERRUN_THROTTLE; cycle++) {
            // clear events
            this->clearEvents();
            // clear port call log
            this->clearPortCalls();
            // clear telemetry log
            this->clearTlm();
            // verify cycle start flag is NOT set on first cycle
            if (0 == cycle) {
                ASSERT_FALSE(this->m_impl.m_cycleStarted);
            } else {
                ASSERT_TRUE(this->m_impl.m_cycleStarted);
            }
            // set flag to cause overrun
            this->m_causeOverrun = true;
            // call active rate group with timer val
            this->invoke_to_CycleIn(0,timer);
            // verify cycle started flag is set
            ASSERT_TRUE(this->m_impl.m_cycleStarted);
            // call doDispatch() for ActiveRateGroup
            this->m_impl.doDispatch();
            // verify cycle started flag is still set
            ASSERT_TRUE(this->m_impl.m_cycleStarted);
            // verify cycle count
            ASSERT_EQ(this->m_impl.m_cycles,static_cast<U32>(cycle)+1);

            // check calls
            for (NATIVE_UINT_TYPE portNum = 0; portNum <
            static_cast<NATIVE_UINT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_RateGroupMemberOut_OutputPort)); portNum++) {
                ASSERT_TRUE(this->m_callLog[portNum].portCalled == true);
            }
            REQUIREMENT("ARG-004");
            // verify overrun event
            ASSERT_EVENTS_RateGroupCycleSlip_SIZE(1);
            ASSERT_EVENTS_RateGroupCycleSlip(0,static_cast<U32>(cycle));

            // verify cycle slip counter is counting up
            ASSERT_EQ(this->m_impl.m_overrunThrottle,cycle+1);

            // check to see if max time was put out
            if (this->tlmHistory_RgMaxTime->size() == 1) {
                ASSERT_TLM_SIZE(2);
            } else {
                ASSERT_TLM_SIZE(1);
            }
            ASSERT_TLM_RgCycleSlips_SIZE(1);
            ASSERT_TLM_RgCycleSlips(0,static_cast<U32>(cycle)+1);

        }

        // Running one more time should show event throttled

        // clear events
        this->clearEvents();
        // clear port call log
        this->clearPortCalls();
        // clear telemetry log
        this->clearTlm();
        // verify cycle start flag is NOT set on first cycle
        ASSERT_TRUE(this->m_impl.m_cycleStarted);
        // set flag to cause overrun
        this->m_causeOverrun = true;
        // call active rate group with timer val
        this->invoke_to_CycleIn(0,timer);
        // verify cycle started flag is set from previous cycle slip
        ASSERT_TRUE(this->m_impl.m_cycleStarted);
        // call doDispatch() for ActiveRateGroup
        this->m_impl.doDispatch();
        // verify cycle started flag is still set
        ASSERT_TRUE(this->m_impl.m_cycleStarted);
        // verify cycle count
        ASSERT_EQ(this->m_impl.m_cycles, static_cast<U32>(ACTIVE_RATE_GROUP_OVERRUN_THROTTLE)+1);
        // check calls
        for (NATIVE_UINT_TYPE portNum = 0; portNum <
        static_cast<NATIVE_UINT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_RateGroupMemberOut_OutputPort)); portNum++) {
            ASSERT_TRUE(this->m_callLog[portNum].portCalled == true);
        }
        // verify overrun event is NOT sent since throttled
        ASSERT_EVENTS_SIZE(0);
        ASSERT_EVENTS_RateGroupCycleSlip_SIZE(0);

        // verify cycle slip counter is counting up
        ASSERT_EQ(this->m_impl.m_overrunThrottle,ACTIVE_RATE_GROUP_OVERRUN_THROTTLE);

        // verify channel updated
        // check to see if max time was put out
        if (this->tlmHistory_RgMaxTime->size() == 1) {
            ASSERT_TLM_SIZE(2);
        } else {
            ASSERT_TLM_SIZE(1);
        }
        ASSERT_TLM_RgCycleSlips_SIZE(1);
        ASSERT_TLM_RgCycleSlips(0, static_cast<U32>(ACTIVE_RATE_GROUP_OVERRUN_THROTTLE)+1);

        // A good cycle should count down the throttle value

        // clear events
        this->clearEvents();
        // clear port call log
        this->clearPortCalls();
        // clear telemetry log
        this->clearTlm();
        // verify cycle start flag is NOT set on first cycle
        ASSERT_TRUE(this->m_impl.m_cycleStarted);
        // set flag to prevent overrun
        this->m_causeOverrun = false;
        // call active rate group with timer val
        this->invoke_to_CycleIn(0,timer);
        // verify cycle started flag is set from previous cycle slip
        ASSERT_TRUE(this->m_impl.m_cycleStarted);
        // call doDispatch() for ActiveRateGroup
        this->m_impl.doDispatch();
        // verify cycle started flag is not set
        ASSERT_FALSE(this->m_impl.m_cycleStarted);
        // verify cycle count
        ASSERT_EQ(this->m_impl.m_cycles,static_cast<U32>(ACTIVE_RATE_GROUP_OVERRUN_THROTTLE)+2);
        // check calls
        for (NATIVE_UINT_TYPE portNum = 0; portNum <
        static_cast<NATIVE_UINT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_RateGroupMemberOut_OutputPort)); portNum++) {
            ASSERT_TRUE(this->m_callLog[portNum].portCalled == true);
        }

        // verify overrun event is NOT sent since good cycle
        ASSERT_EVENTS_SIZE(0);
        ASSERT_EVENTS_RateGroupCycleSlip_SIZE(0);
        // verify cycle slip counter is counting down
        ASSERT_EQ(this->m_impl.m_overrunThrottle,ACTIVE_RATE_GROUP_OVERRUN_THROTTLE-1);

        // verify channel not updated
        ASSERT_TLM_SIZE(0);
        ASSERT_TLM_RgCycleSlips_SIZE(0);

        // Now one more slip to verify event is sent again

        // clear events
        this->clearEvents();
        // clear port call log
        this->clearPortCalls();
        // clear telemetry log
        this->clearTlm();
        // verify cycle start flag is set on cycle
        ASSERT_FALSE(this->m_impl.m_cycleStarted);
        // set flag to cause overrun
        this->m_causeOverrun = true;
        // call active rate group with timer val
        this->invoke_to_CycleIn(0,timer);
        // verify cycle started flag is set from port call
        ASSERT_TRUE(this->m_impl.m_cycleStarted);
        // call doDispatch() for ActiveRateGroup
        this->m_impl.doDispatch();
        // verify cycle started flag is still set
        ASSERT_TRUE(this->m_impl.m_cycleStarted);
        // verify cycle count
        ASSERT_EQ(this->m_impl.m_cycles,static_cast<U32>(ACTIVE_RATE_GROUP_OVERRUN_THROTTLE)+3);
        // check calls
        for (NATIVE_UINT_TYPE portNum = 0; portNum <
        static_cast<NATIVE_UINT_TYPE>(FW_NUM_ARRAY_ELEMENTS(this->m_impl.m_RateGroupMemberOut_OutputPort)); portNum++) {
            ASSERT_TRUE(this->m_callLog[portNum].portCalled == true);
        }
        // verify overrun event is sent
        ASSERT_EVENTS_SIZE(1);
        ASSERT_EVENTS_RateGroupCycleSlip_SIZE(1);
        // verify cycle slip counter is counting up
        ASSERT_EQ(this->m_impl.m_overrunThrottle,ACTIVE_RATE_GROUP_OVERRUN_THROTTLE);

        // verify channel updated
        // check to see if max time was put out
        if (this->tlmHistory_RgMaxTime->size() == 1) {
            ASSERT_TLM_SIZE(2);
        } else {
            ASSERT_TLM_SIZE(1);
        }
        ASSERT_TLM_RgCycleSlips_SIZE(1);
        ASSERT_TLM_RgCycleSlips(0, static_cast<U32>(ACTIVE_RATE_GROUP_OVERRUN_THROTTLE)+2);

    }

    void ActiveRateGroupImplTester::runPingTest() {
        // invoke ping port
        this->invoke_to_PingIn(0,0x123);
        // dispatch message
        this->m_impl.doDispatch();
        // look for return port call
        ASSERT_FROM_PORT_HISTORY_SIZE(1);
        // look for key
        ASSERT_from_PingOut(0, 0x123);

    }

} /* namespace SvcTest */
