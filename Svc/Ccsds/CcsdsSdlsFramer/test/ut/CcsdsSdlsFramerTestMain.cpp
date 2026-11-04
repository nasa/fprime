// ======================================================================
// \title  CcsdsSdlsFramerTestMain.cpp
// \author devin
// \brief  cpp file for CcsdsSdlsFramer component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "Svc/Ccsds/CcsdsSdlsFramer/test/ut/CcsdsSdlsFramerTester.hpp"

using Svc::Ccsds::CcsdsSdlsFramerTester;

// Verify dataIn selects the SA index from the context and forwards data to
// the encryption helper.
TEST(CcsdsSdlsFramer, FrameContextSa) {
    COMMENT("Select the SA index from the frame context and forward data to the encryption helper.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-001");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-002");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::Frame__ContextSa rule;
    rule.apply(tester);
}

// Verify dataIn falls back to the SA_INDEX parameter when the context does
// not specify an SA index.
TEST(CcsdsSdlsFramer, FrameParameterSa) {
    COMMENT("Fall back to the SA_INDEX parameter when the frame context does not specify an SA index.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-001");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-002");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::Frame__ParameterSa rule;
    rule.apply(tester);
}

// Verify an encryption failure raises EncryptionFailed without a direct
// buffer return.
TEST(CcsdsSdlsFramer, FrameEncryptFailure) {
    COMMENT("Raise EncryptionFailed on a bad encrypt status; the failing encryptor returns the buffer.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-004");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::Frame__EncryptFailure rule;
    rule.apply(tester);
}

// Verify encrypted data is framed with a prepended SA index and sent
// downstream, with ownership returned to the encryption helper.
TEST(CcsdsSdlsFramer, EncryptedDataFraming) {
    COMMENT("Allocate a frame buffer, prepend the SA index, send the frame, and return the encrypted buffer.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-003");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::DataFlow__EncryptedData rule;
    rule.apply(tester);
}

// Verify an undersized allocation raises BufferAllocationFailed and drops
// the frame with proper ownership returns.
TEST(CcsdsSdlsFramer, AllocationFailure) {
    COMMENT("Raise BufferAllocationFailed and drop the frame when the allocated buffer is undersized.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-008");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::DataFlow__AllocationFailure rule;
    rule.apply(tester);
}

// Verify ownership returns are routed and comStatus passes through.
TEST(CcsdsSdlsFramer, DataFlowPaths) {
    COMMENT("Route ownership returns in both directions and pass comStatus through.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-005");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-006");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-007");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::DataFlow__DataReturn ruleReturn;
    CcsdsSdlsFramerTester::DataFlow__BufferReturn ruleBufferReturn;
    CcsdsSdlsFramerTester::DataFlow__ComStatus ruleComStatus;
    ruleReturn.apply(tester);
    ruleBufferReturn.apply(tester);
    ruleComStatus.apply(tester);
}

// Randomized test: apply rules in a random sequence for a large number of iterations
TEST(CcsdsSdlsFramer, RandomizedTesting) {
    COMMENT("Apply all rules in a randomized order to exercise interleaved framing and data flow.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-001");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-002");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-003");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-004");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-005");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-006");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-007");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-008");
    const U32 numRulesToApply = 10000;
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::Frame__ContextSa ruleContextSa;
    CcsdsSdlsFramerTester::Frame__ParameterSa ruleParameterSa;
    CcsdsSdlsFramerTester::Frame__EncryptFailure ruleFailure;
    CcsdsSdlsFramerTester::DataFlow__EncryptedData ruleData;
    CcsdsSdlsFramerTester::DataFlow__AllocationFailure ruleAllocation;
    CcsdsSdlsFramerTester::DataFlow__DataReturn ruleReturn;
    CcsdsSdlsFramerTester::DataFlow__BufferReturn ruleBufferReturn;
    CcsdsSdlsFramerTester::DataFlow__ComStatus ruleComStatus;

    STest::Rule<CcsdsSdlsFramerTester>* rules[] = {
        &ruleContextSa,  &ruleParameterSa, &ruleFailure,      &ruleData,
        &ruleAllocation, &ruleReturn,      &ruleBufferReturn, &ruleComStatus,
    };
    STest::RandomScenario<CcsdsSdlsFramerTester> randomScenario("RandomScenario", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<CcsdsSdlsFramerTester> boundedScenario("BoundedScenario", randomScenario, numRulesToApply);
    const U32 numSteps = boundedScenario.run(tester);
    ASSERT_EQ(numSteps, numRulesToApply);
}

// Each failure event has its own quota; exhausting one must not suppress the other.
TEST(CcsdsSdlsFramer, IndependentFailureEventLimits) {
    COMMENT("Bound each failure event independently while preserving buffer returns and ready status.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-009");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::Frame__EncryptFailure encryption;
    CcsdsSdlsFramerTester::DataFlow__AllocationFailure allocation;
    for (U32 i = 0; i < CcsdsSdlsFramerTester::FAILURE_EVENT_LIMIT; ++i) {
        encryption.apply(tester);
        tester.assertEncryptionFailedEvents(1);
    }
    encryption.apply(tester);
    tester.assertEncryptionFailedEvents(0);
    // The exhausted encryption quota does not affect the allocation quota
    for (U32 i = 0; i < CcsdsSdlsFramerTester::FAILURE_EVENT_LIMIT; ++i) {
        allocation.apply(tester);
        tester.assertBufferAllocationFailedEvents(1);
    }
    allocation.apply(tester);
    tester.assertBufferAllocationFailedEvents(0);
    encryption.apply(tester);
    tester.assertEncryptionFailedEvents(0);
}

TEST(CcsdsSdlsFramer, SuccessfulFramesDoNotResetFailureLimits) {
    COMMENT("Interleave successful frames and both failures without resetting their independent quotas.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-009");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::Frame__EncryptFailure encryption;
    CcsdsSdlsFramerTester::DataFlow__AllocationFailure allocation;
    CcsdsSdlsFramerTester::DataFlow__EncryptedData success;
    for (U32 i = 0; i < CcsdsSdlsFramerTester::FAILURE_EVENT_LIMIT; ++i) {
        encryption.apply(tester);
        tester.assertEncryptionFailedEvents(1);
        success.apply(tester);
        allocation.apply(tester);
        tester.assertBufferAllocationFailedEvents(1);
        success.apply(tester);
    }
    encryption.apply(tester);
    tester.assertEncryptionFailedEvents(0);
    success.apply(tester);
    allocation.apply(tester);
    tester.assertBufferAllocationFailedEvents(0);
    success.apply(tester);
    encryption.apply(tester);
    tester.assertEncryptionFailedEvents(0);
}

TEST(CcsdsSdlsFramer, FailureEventLimitsArePerInstance) {
    COMMENT("A newly initialized component has independent, fresh failure-event quotas.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-009");
    for (U32 instance = 0; instance < 2; ++instance) {
        CcsdsSdlsFramerTester tester;
        CcsdsSdlsFramerTester::Frame__EncryptFailure encryption;
        CcsdsSdlsFramerTester::DataFlow__AllocationFailure allocation;
        for (U32 i = 0; i < CcsdsSdlsFramerTester::FAILURE_EVENT_LIMIT; ++i) {
            encryption.apply(tester);
            tester.assertEncryptionFailedEvents(1);
            allocation.apply(tester);
            tester.assertBufferAllocationFailedEvents(1);
        }
        encryption.apply(tester);
        tester.assertEncryptionFailedEvents(0);
        allocation.apply(tester);
        tester.assertBufferAllocationFailedEvents(0);
    }
}

// An exhausted quota re-arms once the throttle period has elapsed, so persistent failures stay observable.
TEST(CcsdsSdlsFramer, FailureEventsResumeAfterThrottlePeriod) {
    COMMENT("Report each failure event again once its throttle period elapses, then bound it again.");
    REQUIREMENT("SVC-CCSDS-SDLS-FRAMER-009");
    CcsdsSdlsFramerTester tester;
    CcsdsSdlsFramerTester::Frame__EncryptFailure encryption;
    CcsdsSdlsFramerTester::DataFlow__AllocationFailure allocation;
    for (U32 i = 0; i < CcsdsSdlsFramerTester::FAILURE_EVENT_LIMIT + 1; ++i) {
        encryption.apply(tester);
        allocation.apply(tester);
    }
    encryption.apply(tester);
    tester.assertEncryptionFailedEvents(0);
    allocation.apply(tester);
    tester.assertBufferAllocationFailedEvents(0);
    // Just short of the period both events remain suppressed
    tester.advanceTestTime(CcsdsSdlsFramerTester::FAILURE_EVENT_PERIOD_SECONDS - 1);
    encryption.apply(tester);
    tester.assertEncryptionFailedEvents(0);
    allocation.apply(tester);
    tester.assertBufferAllocationFailedEvents(0);
    // At the period boundary both events are reported again and their quotas re-arm
    tester.advanceTestTime(1);
    for (U32 i = 0; i < CcsdsSdlsFramerTester::FAILURE_EVENT_LIMIT; ++i) {
        encryption.apply(tester);
        tester.assertEncryptionFailedEvents(1);
        allocation.apply(tester);
        tester.assertBufferAllocationFailedEvents(1);
    }
    encryption.apply(tester);
    tester.assertEncryptionFailedEvents(0);
    allocation.apply(tester);
    tester.assertBufferAllocationFailedEvents(0);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
