// ======================================================================
// \title  IsValidTest.cpp
// \author T. Chieu, R. Bocchino
// \brief  cpp file for IsValidTest class
// ======================================================================

#include "FppTest/enum/IntervalEnumAc.hpp"

#include "gtest/gtest.h"

namespace FppTest {
namespace Enum {
// Test boundary values for enum isValid() function
TEST(IsValidTest, IntervalEnum) {
    Interval::SerialType es = static_cast<Interval::SerialType>(-1);
    ASSERT_FALSE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(0);
    ASSERT_TRUE(Interval::isValid(es));
    Interval interval(static_cast<Interval::T>(es));
    ASSERT_TRUE(interval.isValid());

    es = static_cast<Interval::SerialType>(1);
    ASSERT_FALSE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(2);
    ASSERT_FALSE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(3);
    ASSERT_TRUE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(5);
    ASSERT_TRUE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(6);
    ASSERT_FALSE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(10);
    ASSERT_TRUE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(99);
    ASSERT_FALSE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(100);
    ASSERT_TRUE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(101);
    ASSERT_TRUE(Interval::isValid(es));

    es = static_cast<Interval::SerialType>(102);
    ASSERT_FALSE(Interval::isValid(es));
}
}  // namespace Enum
}  // namespace FppTest
