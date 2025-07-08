#ifndef FW_TIME_INTERVAL_TESTER_HPP
#define FW_TIME_INTERVAL_TESTER_HPP

#include <Fw/Time/TimeInterval.hpp>
#include <gtest/gtest.h>

namespace Fw {
    class TimeIntervalTester {
    public:
        // ----------------------------------------------------------------------
        // Construction and destruction
        // ----------------------------------------------------------------------
        TimeIntervalTester();
        ~TimeIntervalTester();

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------
        void test_TimeIntervalInstantiateTest();
        void test_TimeIntervalComparisonTest();
        void test_TimeIntervalCompareStaticTest();
        void test_TimeIntervalAdditionTest();
        void test_TimeIntervalSubtractionTest();
    };
} // namespace Fw

#endif // FW_TIME_INTERVAL_TESTER_HPP
