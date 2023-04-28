// ======================================================================
// \title  TestMain.cpp
// \author T. Chieu
// \brief  main cpp file for FPP port tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include "Tester.hpp"
#include "FppTest/types/FormalParamTypes.hpp"
#include "FppTest/typed_tests/PortTest.hpp"
#include "FppTest/typed_tests/StringTest.hpp"
#include "FppTest/types/StringArgsPortAc.hpp"

#include "gtest/gtest.h"

// Typed port tests
using TypedPortTestImplementations = ::testing::Types<
    FppTest::Types::NoParams,
    FppTest::Types::PrimitiveParams,
    FppTest::Types::PortStringParams,
    FppTest::Types::EnumParams,
    FppTest::Types::ArrayParams,
    FppTest::Types::StructParams,
    FppTest::Types::NoParamReturn,
    FppTest::Types::PrimitiveReturn,
    FppTest::Types::EnumReturn,
    FppTest::Types::ArrayReturn,
    FppTest::Types::StructReturn
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                               TypedPortTest,
                               TypedPortTestImplementations);

// Serial port tests
using SerialPortTestImplementations = ::testing::Types<
    FppTest::Types::NoParams,
    FppTest::Types::PrimitiveParams,
    FppTest::Types::PortStringParams,
    FppTest::Types::EnumParams,
    FppTest::Types::ArrayParams,
    FppTest::Types::StructParams
//    FppTest::Types::SerialParams
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                               SerialPortTest,
                               SerialPortTestImplementations);

// String tests
using StringTestImplementations = ::testing::Types<
    StringArgsPortStrings::StringSize80,
    StringArgsPortStrings::StringSize100
>;

INSTANTIATE_TYPED_TEST_SUITE_P(Array, StringTest, StringTestImplementations);

template<>
U32 FppTest::String::getSize<StringArgsPortStrings::StringSize100>() {
    return 100;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
