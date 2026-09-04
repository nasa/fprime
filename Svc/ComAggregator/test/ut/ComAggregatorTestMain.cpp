// ======================================================================
// \title  ComAggregatorTestMain.cpp
// \author lestarch
// \brief  cpp file for ComAggregator component test main function
// ======================================================================

#include "ComAggregatorTester.hpp"
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
    Svc::ComAggregatorTester tester;
    tester.test_spanning_split_two();
}

TEST(Spanning, SpanCompleteMiddleFrame) {
    Svc::ComAggregatorTester tester;
    tester.test_spanning_three_frames();
}

TEST(Spanning, IdlePacketSpansFrames) {
    Svc::ComAggregatorTester tester;
    tester.test_spanning_idle_span();
}

TEST(Assertions, ConfigureAfterFill) {
    Svc::ComAggregatorTester tester;
    tester.test_configure_after_fill_asserts();
}

TEST(Assertions, OversizeHoldWithoutSpanning) {
    Svc::ComAggregatorTester tester;
    tester.test_initial();
    tester.test_fill_multi();
    tester.test_oversize_hold_asserts();
}

// Randomized spanning: apply rules in a random sequence against the shadow byte-stream model
TEST(Spanning, RandomizedTesting) {
    const U32 numRulesToApply = 10000;
    Svc::ComAggregatorTester tester;
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

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
