/*
 * RateGroupDriverImplTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Svc/RateGroupDriver/test/ut/RateGroupDriverImplTester.hpp>
#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>

#include <Fw/Test/UnitTest.hpp>

namespace Svc {

    void RateGroupDriverImplTester::init(NATIVE_INT_TYPE instance) {
        RateGroupDriverGTestBase::init();
    }

    RateGroupDriverImplTester::RateGroupDriverImplTester(Svc::RateGroupDriver& inst) :
        RateGroupDriverGTestBase("testerbase",100),
            m_impl(inst) {
        this->clearPortCalls();
    }

    void RateGroupDriverImplTester::clearPortCalls() {
        memset(this->m_portCalls,0,sizeof(this->m_portCalls));
    }


    RateGroupDriverImplTester::~RateGroupDriverImplTester() {
    }

    void RateGroupDriverImplTester::from_CycleOut_handler(NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {
        this->m_portCalls[portNum] = true;
    }

    void RateGroupDriverImplTester::runSchedNominal(Svc::RateGroupDriver::DividerSet dividersSet, NATIVE_INT_TYPE numDividers) {

        TEST_CASE(106.1.1,"Nominal Execution");
        COMMENT(
                "Call the port with enough ticks that the internal rollover value will roll over.\n"
                "Verify that the output ports are being called correctly.\n"
                );

        NATIVE_INT_TYPE expected_rollover = 1;

        for (NATIVE_INT_TYPE div = 0; div < numDividers; div++) {
            expected_rollover *= dividersSet.dividers[div].divisor;
        }

        ASSERT_EQ(expected_rollover,this->m_impl.m_rollover);

        NATIVE_INT_TYPE iters = expected_rollover*10;

        REQUIREMENT("RGD-001");

        for (NATIVE_INT_TYPE cycle = 0; cycle < iters; cycle++) {
            this->clearPortCalls();
            TimerVal t;
            this->invoke_to_CycleIn(0,t);
            // make sure ticks are counting correctly
            ASSERT_EQ((cycle+1)%expected_rollover,this->m_impl.m_ticks);
            // check for various intervals
            for (NATIVE_INT_TYPE div = 0; div < numDividers; div++) {
                if (cycle % dividersSet.dividers[div].divisor == dividersSet.dividers[div].offset) {
                    EXPECT_TRUE(this->m_portCalls[div]);
                } else {
                    EXPECT_FALSE(this->m_portCalls[div]);
                }
            }
        }

    }

} /* namespace SvcTest */
