// ======================================================================
// \title  main.cpp
// \author T. Chieu
// \brief  main cpp file for FPP struct tests
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/struct/MultiStringSerializableAc.hpp"
#include "FppTest/struct/NonPrimitiveSerializableAc.hpp"
#include "FppTest/typed_tests/StringTest.hpp"
#include "Fw/Types/StringTemplate.hpp"

#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

// Instantiate string tests for structs
using StringTestImplementations = ::testing::Types<
    Fw::StringTemplate<80>,
    Fw::StringTemplate<50>,
    Fw::StringTemplate<60>
>;
INSTANTIATE_TYPED_TEST_SUITE_P(Struct, StringTest, StringTestImplementations);

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
