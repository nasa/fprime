// ======================================================================
// \title  ComAggregatorTestMain.cpp
// \author lestarch
// \brief  cpp file for ComAggregator component test main function
// ======================================================================

#include "ComAggregatorTester.hpp"

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
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
