// ======================================================================
// \title  ComAggregatorTestMain.cpp
// \author lestarch
// \brief  cpp file for ComAggregator component test main function
// ======================================================================

#include "ComAggregatorTester.hpp"
#include "STest/Random/Random.hpp"

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

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
