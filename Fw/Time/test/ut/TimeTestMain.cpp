/*
 * TimeTest.cpp
 *
 *  Created on: Apr 22, 2016
 *      Author: tcanham
 */

#include <gtest/gtest.h>
#include "TimeIntervalTester.hpp"
#include "TimeTester.hpp"

// Time tests
TEST(TimeTestNominal, InstantiateTest) {
    Fw::TimeTester tester;
}

TEST(TimeTestNominal, MathTest) {
    Fw::TimeTester tester;
    tester.test_MathTest();
}

TEST(TimeTestNominal, CopyTest) {
    Fw::TimeTester tester;
    tester.test_CopyTest();
}

TEST(TimeTestNominal, ZeroTimeEquality) {
    Fw::TimeTester tester;
    tester.test_ZeroTimeEquality();
}

// TimeInterval tests
TEST(TimeIntervalTestNominal, test_TimeIntervalInstantiateTest) {
    Fw::TimeIntervalTester tester;
    tester.test_TimeIntervalInstantiateTest();
}

TEST(TimeIntervalTestNominal, test_TimeIntervalComparisonTest) {
    Fw::TimeIntervalTester tester;
    tester.test_TimeIntervalComparisonTest();
}

TEST(TimeIntervalTestNominal, test_TimeIntervalCompareStaticTest) {
    Fw::TimeIntervalTester tester;
    tester.test_TimeIntervalCompareStaticTest();
}

TEST(TimeIntervalTestNominal, test_TimeIntervalAdditionTest) {
    Fw::TimeIntervalTester tester;
    tester.test_TimeIntervalAdditionTest();
}

TEST(TimeIntervalTestNominal, test_TimeIntervalSubtractionTest) {
    Fw::TimeIntervalTester tester;
    tester.test_TimeIntervalSubtractionTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
