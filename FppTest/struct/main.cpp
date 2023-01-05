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

#include "FppTest/struct/NonPrimitiveSerializableAc.hpp"
#include "FppTest/struct/MultiStringSerializableAc.hpp"
#include "FppTest/typed_tests/StringTest.hpp"

#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

// Instantiate string tests for structs
using StringTestImplementations = ::testing::Types<
    NonPrimitive::StringSize80,
    MultiString::StringSize50,
    MultiString::StringSize60,
    MultiString::StringSize80
>;
INSTANTIATE_TYPED_TEST_SUITE_P(Struct, StringTest, StringTestImplementations);

template<>
U32 FppTest::String::getSize<MultiString::StringSize50>() {
    return 50;
}

template<>
U32 FppTest::String::getSize<MultiString::StringSize60>() {
    return 60;
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
