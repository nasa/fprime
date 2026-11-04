// ======================================================================
// \title  AlgorithmsTester.cpp
// \brief  cpp file for Algorithms unit tests
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <gtest/gtest.h>
#include <Utils/Algorithms.hpp>

TEST(AlgorithmsTest, MathGcdLcm) {
    // GCD properties
    EXPECT_EQ(0, Utils::gcd(0, 0));
    EXPECT_EQ(5, Utils::gcd(0, 5));
    EXPECT_EQ(5, Utils::gcd(5, 0));
    EXPECT_EQ(6, Utils::gcd(12, 18));
    EXPECT_EQ(6, Utils::gcd(18, 12));
    EXPECT_EQ(1, Utils::gcd(17, 19));
    EXPECT_EQ(25, Utils::gcd(100, 25));

    // LCM properties
    EXPECT_EQ(1, Utils::lcm(1, 1));
    EXPECT_EQ(12, Utils::lcm(4, 6));
    EXPECT_EQ(36, Utils::lcm(12, 18));
    EXPECT_EQ(20, Utils::lcm(10, 20));
    EXPECT_EQ(2000, Utils::lcm(1000, 2000));

    // Also verify via explicit Algorithms namespace
    EXPECT_EQ(6, Utils::Algorithms::gcd(12, 18));
    EXPECT_EQ(36, Utils::Algorithms::lcm(12, 18));
}
