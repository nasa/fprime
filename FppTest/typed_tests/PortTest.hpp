// ======================================================================
// \title  PortTest.hpp
// \author T. Chieu
// \brief  hpp file for PortTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_PORT_TEST_HPP
#define FPP_TEST_PORT_TEST_HPP

#include "Tester.hpp"

#include "gtest/gtest.h"

template <typename PortType>
class PortTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(PortTest);

TYPED_TEST_P(PortTest, TypedPort) {
    Tester tester;
    TypeParam port;

    tester.invoke(0, port);

    tester.check_history(0, port);
}

// Register all test patterns
REGISTER_TYPED_TEST_SUITE_P(PortTest,
    TypedPort
);

#endif
