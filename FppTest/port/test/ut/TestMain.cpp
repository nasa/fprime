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
#include "FppTest/port/PortTypes.hpp"
#include "FppTest/typed_tests/PortTest.hpp"
#include "FppTest/typed_tests/StringTest.hpp"
#include "FppTest/port/StringArgsPortAc.hpp"

#include "gtest/gtest.h"

// Typed port tests
using TypedPortTestImplementations = ::testing::Types<
    FppTest::Port::NoArgsPort,
    FppTest::Port::PrimitiveArgsPort,
    FppTest::Port::StringArgsPort,
    FppTest::Port::EnumArgsPort,
    FppTest::Port::ArrayArgsPort,
    FppTest::Port::StructArgsPort,
    FppTest::Port::NoArgsReturnPort,
    FppTest::Port::PrimitiveReturnPort,
    FppTest::Port::EnumReturnPort,
    FppTest::Port::ArrayReturnPort,
    FppTest::Port::StructReturnPort
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                               TypedPortTest,
                               TypedPortTestImplementations);

// Serial port tests
using SerialPortTestImplementations = ::testing::Types<
    FppTest::Port::NoArgsPort,
    FppTest::Port::PrimitiveArgsPort,
    FppTest::Port::StringArgsPort,
    FppTest::Port::EnumArgsPort,
    FppTest::Port::ArrayArgsPort,
    FppTest::Port::StructArgsPort
//    FppTest::Port::SerialArgsPort
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
