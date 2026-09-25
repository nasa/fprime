/*
 * RateGroupDriverTester.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: tcanham
 */

#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Svc/RateGroupDriver/RateGroupDriver.hpp>
#include <Svc/RateGroupDriver/test/ut/RateGroupDriverImplTester.hpp>

#include <gtest/gtest.h>
#include <limits>

#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

void connectPorts(Svc::RateGroupDriver& impl, Svc::RateGroupDriverImplTester& tester) {
    for (FwIndexType i = 0; i < Svc::RateGroupDriver::DIVIDER_SIZE; i++) {
        impl.set_CycleOut_OutputPort(i, tester.get_from_CycleOut(i));
    }

    tester.connect_to_CycleIn(0, impl.get_CycleIn_InputPort(0));
}

TEST(RateGroupDriverTest, MathGcdLcm) {
    // GCD properties
    EXPECT_EQ(0, Svc::RateGroupDriver::gcd(0, 0));
    EXPECT_EQ(5, Svc::RateGroupDriver::gcd(0, 5));
    EXPECT_EQ(5, Svc::RateGroupDriver::gcd(5, 0));
    EXPECT_EQ(6, Svc::RateGroupDriver::gcd(12, 18));
    EXPECT_EQ(6, Svc::RateGroupDriver::gcd(18, 12));
    EXPECT_EQ(1, Svc::RateGroupDriver::gcd(17, 19));
    EXPECT_EQ(25, Svc::RateGroupDriver::gcd(100, 25));

    // LCM properties
    EXPECT_EQ(1, Svc::RateGroupDriver::lcm(1, 1));
    EXPECT_EQ(12, Svc::RateGroupDriver::lcm(4, 6));
    EXPECT_EQ(36, Svc::RateGroupDriver::lcm(12, 18));
    EXPECT_EQ(20, Svc::RateGroupDriver::lcm(10, 20));
    EXPECT_EQ(2000, Svc::RateGroupDriver::lcm(1000, 2000));
}

TEST(RateGroupDriverTest, NominalSchedule) {
    Svc::RateGroupDriver::DividerSet dividersSet{};
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(Svc::RateGroupDriver::DIVIDER_SIZE); i++) {
        dividersSet.dividers[i] = {static_cast<FwSizeType>(i + 1), static_cast<FwSizeType>(i % 2)};
    }

    Svc::RateGroupDriver impl("RateGroupDriver");
    impl.configure(dividersSet);

    Svc::RateGroupDriverImplTester tester(impl);

    tester.init();
    impl.init();

    // connect ports
    connectPorts(impl, tester);

    tester.runSchedNominal(dividersSet, FW_NUM_ARRAY_ELEMENTS(dividersSet.dividers));
}

TEST(RateGroupDriverTest, ReconfigureSchedule) {
    Svc::RateGroupDriver::DividerSet dividersSet{};
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(Svc::RateGroupDriver::DIVIDER_SIZE); i++) {
        dividersSet.dividers[i] = {static_cast<FwSizeType>(i + 1), static_cast<FwSizeType>(i % 2)};
    }

    // First configure with different (larger) divisors, then reconfigure: the second
    // configure must fully replace the first (rollover resets, no product carryover)
    Svc::RateGroupDriver::DividerSet firstSet{};
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(Svc::RateGroupDriver::DIVIDER_SIZE); i++) {
        firstSet.dividers[i] = {static_cast<FwSizeType>(2 * (i + 1)), 0};
    }

    Svc::RateGroupDriver impl("RateGroupDriver");
    impl.configure(firstSet);
    impl.configure(dividersSet);

    Svc::RateGroupDriverImplTester tester(impl);

    tester.init();
    impl.init();

    connectPorts(impl, tester);

    tester.runSchedNominal(dividersSet, FW_NUM_ARRAY_ELEMENTS(dividersSet.dividers));
}

TEST(RateGroupDriverTest, LcmRolloverCommonFactors) {
    // Use divisors with common factors: 2, 4, 8
    // The product would be 64, but the LCM is 8
    Svc::RateGroupDriver::DividerSet dividersSet{};
    dividersSet.dividers[0] = {2, 0};
    if (Svc::RateGroupDriver::DIVIDER_SIZE > 1) {
        dividersSet.dividers[1] = {4, 1};
    }
    if (Svc::RateGroupDriver::DIVIDER_SIZE > 2) {
        dividersSet.dividers[2] = {8, 3};
    }

    Svc::RateGroupDriver impl("RateGroupDriver");
    impl.configure(dividersSet);

    Svc::RateGroupDriverImplTester tester(impl);
    tester.init();
    impl.init();

    connectPorts(impl, tester);

    // Verify rollover matches literal LCM
    const FwSizeType expectedLcm = (Svc::RateGroupDriver::DIVIDER_SIZE > 2)   ? 8
                                   : (Svc::RateGroupDriver::DIVIDER_SIZE > 1) ? 4
                                                                              : 2;
    EXPECT_EQ(expectedLcm, tester.getRollover());

    tester.runSchedNominal(dividersSet, Svc::RateGroupDriver::DIVIDER_SIZE);
}

TEST(RateGroupDriverTest, LargeDivisorsNoOverflow) {
    // Select divisors such that their product would overflow FwSizeType max,
    // but their LCM easily fits in FwSizeType without overflow (#5987)
    const FwSizeType base = (std::numeric_limits<FwSizeType>::max() / 4) + 1;
    Svc::RateGroupDriver::DividerSet dividersSet{};

    // All divisors are multiples of base / 2: {base / 2, base, base}
    // Product would be (base^3) / 2 which overflows FwSizeType max,
    // but LCM is simply base
    dividersSet.dividers[0] = {base / 2, 0};
    for (FwIndexType i = 1; i < static_cast<FwIndexType>(Svc::RateGroupDriver::DIVIDER_SIZE); i++) {
        dividersSet.dividers[i] = {base, 0};
    }

    Svc::RateGroupDriver impl("RateGroupDriver");
    // configure() must succeed without triggering an overflow assertion
    impl.configure(dividersSet);

    Svc::RateGroupDriverImplTester tester(impl);
    EXPECT_EQ(base, tester.getRollover());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
