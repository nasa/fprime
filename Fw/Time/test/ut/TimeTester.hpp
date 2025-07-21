#ifndef FW_TIME_TESTER_HPP
#define FW_TIME_TESTER_HPP

#include <Fw/Time/Time.hpp>
#include <gtest/gtest.h>

namespace Fw {
    class TimeTester {
    public:
        // ----------------------------------------------------------------------
        // Construction and destruction
        // ----------------------------------------------------------------------
        TimeTester();
        ~TimeTester();

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------
        void test_InstantiateTest();
        void test_MathTest();
        void test_CopyTest();
        void test_ZeroTimeEquality();
    };
} // namespace Fw

#endif // FW_TIME_TESTER_HPP
