// ======================================================================
// \title  IsValidTest.cpp
// \author T. Chieu
// \brief  cpp file for IsValidTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/enum/IntervalEnumAc.hpp"

#include "gtest/gtest.h"

// Test boundary values for enum isValid() function
TEST(IsValidTest, IntervalEnum) {
    Interval e = static_cast<Interval::T>(-1);
    ASSERT_FALSE(e.isValid());

    e = static_cast<Interval::T>(0);
    ASSERT_TRUE(e.isValid());

    e = static_cast<Interval::T>(1);
    ASSERT_FALSE(e.isValid());

    e = static_cast<Interval::T>(2);
    ASSERT_FALSE(e.isValid());

    e = static_cast<Interval::T>(3);
    ASSERT_TRUE(e.isValid());

    e = static_cast<Interval::T>(5);
    ASSERT_TRUE(e.isValid());

    e = static_cast<Interval::T>(6);
    ASSERT_FALSE(e.isValid());

    e = static_cast<Interval::T>(10);
    ASSERT_TRUE(e.isValid());

    e = static_cast<Interval::T>(99);
    ASSERT_FALSE(e.isValid());

    e = static_cast<Interval::T>(100);
    ASSERT_TRUE(e.isValid());

    e = static_cast<Interval::T>(101);
    ASSERT_TRUE(e.isValid());

    e = static_cast<Interval::T>(102);
    ASSERT_FALSE(e.isValid());
}
