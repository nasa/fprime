// ======================================================================
// \title  ComAggregatorTestMain.cpp
// \author lestarch
// \brief  cpp file for ComAggregator component test main function
// ======================================================================

#include "ComAggregatorTester.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Random/Random.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

TEST(Nominal, Initial) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
}

TEST(Nominal, Fill) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill();
}

TEST(Nominal, MultiFill) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
}

TEST(Nominal, Full) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_full();
}

TEST(Nominal, ExactlyFull) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_exactly_full();
}

TEST(Nominal, Timeout) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_timeout();
}

TEST(OffNominal, TimeoutEmpty) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_timeout_zero();
    tester.test_fill_multi();
    tester.test_full();
}

TEST(OffNominal, TimeoutOverflowPrevention) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_timeout_overflow_prevention();
    // Now ensure normal operation resumes
    tester.test_fill_multi();
    tester.test_timeout();
}

TEST(Nominal, HoldWhileWaiting) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_hold_while_waiting();
}

TEST(Nominal, Clear) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_full();
    tester.test_fill_multi();
    tester.test_timeout();
    tester.test_fill_multi();
    tester.test_full();
}

TEST(Spanning, SplitAcrossTwoFrames) {
    Svc::ComAggregatorTester tester(Svc::ComAggregatorTester::DEFAULT_AGGREGATION_SIZE, true);
    tester.test_spanning_split_two();
}

TEST(Spanning, SpanCompleteMiddleFrame) {
    Svc::ComAggregatorTester tester(Svc::ComAggregatorTester::DEFAULT_AGGREGATION_SIZE, true);
    tester.test_spanning_three_frames();
}

TEST(Spanning, IdlePacketSpansFrames) {
    Svc::ComAggregatorTester tester(Svc::ComAggregatorTester::DEFAULT_AGGREGATION_SIZE, true);
    tester.test_spanning_idle_span();
}

TEST(Assertions, ConfigureAfterFill) {
    Svc::ComAggregatorTester tester;
    tester.test_configure_after_fill_asserts();
}

TEST(Assertions, ConfigureInvalidSize) {
    Svc::ComAggregatorTester tester;
    tester.test_configure_invalid_size_asserts();
}

TEST(Assertions, ConfigureAllocationFailure) {
    Svc::ComAggregatorTester tester;
    tester.test_configure_allocation_failure_asserts();
}

TEST(Lifecycle, Cleanup) {
    Svc::ComAggregatorTester tester;
    tester.test_cleanup();
}

// Per-instance aggregation size: a random size at or above the non-spanning minimum, below the default
TEST(PerInstance, SmallAggregationSize) {
    const FwSizeType size =
        STest::Pick::lowerUpper(static_cast<U32>(Svc::ComAggregator::MIN_NON_SPANNING_AGGREGATION_SIZE),
                                static_cast<U32>(Svc::ComAggregatorTester::DEFAULT_AGGREGATION_SIZE - 1));
    Svc::ComAggregatorTester tester(size);
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_full();
    tester.test_fill_multi();
    tester.test_timeout();
    tester.test_fill_multi();
    tester.test_exactly_full();
}

// Per-instance aggregation size: the largest size spanning supports (TM First Header Pointer range)
TEST(PerInstance, LargestSpanningSize) {
    Svc::ComAggregatorTester tester(static_cast<FwSizeType>(Svc::Ccsds::TMSubfields::FHP_IDLE_DATA_ONLY), true);
    tester.test_spanning_split_two();
}

TEST(PerInstance, LargeNonSpanningSize) {
    Svc::ComAggregatorTester tester(4096);
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_full();
    tester.test_fill_multi();
    tester.test_timeout();
}

TEST(Assertions, OversizeHoldWithoutSpanning) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_oversize_hold_asserts();
}

TEST(Assertions, OversizeFillWithoutSpanning) {
    Svc::ComAggregatorTester tester;
    tester.test_oversize_fill_asserts();
}

TEST(Spanning, FailureDropsSplitRemainder) {
    Svc::ComAggregatorTester tester(Svc::ComAggregatorTester::DEFAULT_AGGREGATION_SIZE, true);
    tester.test_spanning_failure_drops_split_remainder();
}

// Randomized spanning: apply rules in a random sequence against the shadow byte-stream model
static void run_spanning_randomized(Svc::ComAggregatorTester& tester) {
    const U32 numRulesToApply = 10000;
    tester.spanning_rbt_start();
    Svc::ComAggregatorTester::Spanning__SendPacket ruleSendPacket;
    Svc::ComAggregatorTester::Spanning__SendPacketWhileWaiting ruleSendPacketWhileWaiting;
    Svc::ComAggregatorTester::Spanning__Timeout ruleTimeout;
    Svc::ComAggregatorTester::Spanning__StatusFailure ruleStatusFailure;
    Svc::ComAggregatorTester::Spanning__ReturnAndStatus ruleReturnAndStatus;

    STest::Rule<Svc::ComAggregatorTester>* rules[] = {
        &ruleSendPacket, &ruleSendPacketWhileWaiting, &ruleTimeout, &ruleStatusFailure, &ruleReturnAndStatus,
    };

    STest::RandomScenario<Svc::ComAggregatorTester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<Svc::ComAggregatorTester> bounded("Bounded Random Rules Scenario", random, numRulesToApply);
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
    tester.spanning_rbt_finish();
}

TEST(Spanning, RandomizedTesting) {
    Svc::ComAggregatorTester tester(Svc::ComAggregatorTester::DEFAULT_AGGREGATION_SIZE, true);
    run_spanning_randomized(tester);
}

// Randomized spanning with a per-instance aggregation size: small sizes make idle packets span often
TEST(PerInstance, RandomizedSpanningSmallSize) {
    const FwSizeType size = STest::Pick::lowerUpper(static_cast<U32>(Svc::Ccsds::Utils::IdlePacket::MIN_SIZE) + 1, 128);
    Svc::ComAggregatorTester tester(size, true);
    run_spanning_randomized(tester);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
