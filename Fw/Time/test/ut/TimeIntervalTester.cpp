#include "TimeIntervalTester.hpp"
#include <iostream>

namespace Fw {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TimeIntervalTester::TimeIntervalTester() {}

TimeIntervalTester::~TimeIntervalTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TimeIntervalTester::test_TimeIntervalInstantiateTest() {
    Fw::TimeInterval time(1, 2);
    ASSERT_EQ(time.getSeconds(), 1);
    ASSERT_EQ(time.getUSeconds(), 2);
    std::cout << time << std::endl;

    Fw::TimeInterval time2(time);
    ASSERT_EQ(time.getSeconds(), 1);
    ASSERT_EQ(time.getUSeconds(), 2);
    std::cout << time2 << std::endl;
}

void TimeIntervalTester::test_TimeIntervalComparisonTest() {
    Fw::TimeInterval t1(1, 0);
    Fw::TimeInterval t2(1, 0);
    Fw::TimeInterval t3(2, 0);
    Fw::TimeInterval t4(1, 500000);

    // Equality operators
    ASSERT_TRUE(t1 == t2);
    ASSERT_FALSE(t1 == t3);
    ASSERT_FALSE(t1 == t4);

    // Inequality operators
    ASSERT_FALSE(t1 != t2);
    ASSERT_TRUE(t1 != t3);
    ASSERT_TRUE(t1 != t4);

    // Greater than operators
    ASSERT_TRUE(t3 > t1);
    ASSERT_FALSE(t1 > t3);
    ASSERT_TRUE(t4 > t1);

    // Less than operators
    ASSERT_TRUE(t1 < t3);
    ASSERT_FALSE(t3 < t1);
    ASSERT_TRUE(t1 < t4);

    // Greater than or equal operators
    ASSERT_TRUE(t1 >= t2);
    ASSERT_TRUE(t3 >= t1);
    ASSERT_FALSE(t1 >= t3);

    // Less than or equal operators
    ASSERT_TRUE(t1 <= t2);
    ASSERT_TRUE(t1 <= t3);
    ASSERT_FALSE(t3 <= t1);
}

void TimeIntervalTester::test_TimeIntervalAdditionTest() {
    Fw::TimeInterval t1(1, 500000);
    Fw::TimeInterval t2(2, 600000);

    // Test instance add method
    t1.add(3, 700000);
    // 1 + 3 = 4s, 500000us + 700000us = 1s + 200000us -> 5s, 200000us
    ASSERT_EQ(t1.getSeconds(), 5);
    ASSERT_EQ(t1.getUSeconds(), 200000);

    // Test static add method
    Fw::TimeInterval result = Fw::TimeInterval::add(t1, t2);
    // 5 + 2 = 7s, 200000us + 600000us = 800000us
    ASSERT_EQ(result.getSeconds(), 7);
    ASSERT_EQ(result.getUSeconds(), 800000);
}

void TimeIntervalTester::test_TimeIntervalCompareStaticTest() {
    Fw::TimeInterval t1(1, 0);
    Fw::TimeInterval t2(1, 0);
    Fw::TimeInterval t3(2, 0);
    Fw::TimeInterval t4(1, 500000);

    // Test static compare method
    ASSERT_EQ(Fw::TimeInterval::compare(t1, t2), Fw::TimeInterval::EQ);
    ASSERT_EQ(Fw::TimeInterval::compare(t1, t3), Fw::TimeInterval::LT);
    ASSERT_EQ(Fw::TimeInterval::compare(t3, t1), Fw::TimeInterval::GT);
    ASSERT_EQ(Fw::TimeInterval::compare(t1, t4), Fw::TimeInterval::LT);
    ASSERT_EQ(Fw::TimeInterval::compare(t4, t1), Fw::TimeInterval::GT);
}

void TimeIntervalTester::test_TimeIntervalSubtractionTest() {
    Fw::TimeInterval t1(5, 500000);
    Fw::TimeInterval t2(2, 300000);
    Fw::TimeInterval result1 = Fw::TimeInterval::sub(t1, t2);
    // 5s - 2s = 3s, 500000us - 300000us = 200000us
    ASSERT_EQ(result1.getSeconds(), 3);
    ASSERT_EQ(result1.getUSeconds(), 200000);

    // should be the same due to absolute value
    Fw::TimeInterval result2 = Fw::TimeInterval::sub(t2, t1);
    ASSERT_EQ(result2.getSeconds(), 3);
    ASSERT_EQ(result2.getUSeconds(), 200000);

    Fw::TimeInterval t3(5, 200000);
    Fw::TimeInterval t4(2, 500000);
    Fw::TimeInterval result3 = Fw::TimeInterval::sub(t3, t4);
    // 5s - 2s = 3s, 200000us - 500000us requires borrow
    // So it's 2s + 700000us
    ASSERT_EQ(result3.getSeconds(), 2);
    ASSERT_EQ(result3.getUSeconds(), 700000);

    Fw::TimeInterval t5(3, 400000);
    Fw::TimeInterval t6(3, 400000);
    Fw::TimeInterval result4 = Fw::TimeInterval::sub(t5, t6);
    ASSERT_EQ(result4.getSeconds(), 0);
    ASSERT_EQ(result4.getUSeconds(), 0);
}

}  // namespace Fw
