// ======================================================================
// \title  Tests.cpp
// \author T. Chieu
// \brief  cpp file for component tests
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/component/active/StringArgsPortAc.hpp"
#include "FppTest/component/types/FormalParamTypes.hpp"
#include "FppTest/typed_tests/ComponentTest.hpp"
#include "FppTest/typed_tests/PortTest.hpp"
#include "FppTest/typed_tests/StringTest.hpp"

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(TypedAsyncPortTest);
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(SerialAsyncPortTest);
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ComponentAsyncCommandTest);
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ComponentInternalInterfaceTest);

// Typed port tests
using TypedPortTestImplementations = ::testing::Types<FppTest::Types::NoParams,
                                                      FppTest::Types::PrimitiveParams,
                                                      FppTest::Types::PortStringParams,
                                                      FppTest::Types::EnumParams,
                                                      FppTest::Types::ArrayParams,
                                                      FppTest::Types::StructParams,
                                                      FppTest::Types::NoParamReturn,
                                                      FppTest::Types::PrimitiveReturn,
                                                      FppTest::Types::EnumReturn,
                                                      FppTest::Types::ArrayReturn,
                                                      FppTest::Types::StructReturn>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, TypedPortTest, TypedPortTestImplementations);

// Serial port tests
using SerialPortTestImplementations = ::testing::Types<FppTest::Types::NoParams,
                                                       FppTest::Types::PrimitiveParams,
                                                       FppTest::Types::PortStringParams,
                                                       FppTest::Types::EnumParams,
                                                       FppTest::Types::ArrayParams,
                                                       FppTest::Types::StructParams>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, SerialPortTest, SerialPortTestImplementations);

// String tests
using StringTestImplementations =
    ::testing::Types<StringArgsPortStrings::StringSize80, StringArgsPortStrings::StringSize100>;

INSTANTIATE_TYPED_TEST_SUITE_P(Array, StringTest, StringTestImplementations);

template <>
U32 FppTest::String::getSize<StringArgsPortStrings::StringSize100>() {
    return 100;
}

// Command tests
using CommandTestImplementations = ::testing::Types<FppTest::Types::NoParams,
                                                    FppTest::Types::PrimitiveParams,
                                                    FppTest::Types::CmdStringParams,
                                                    FppTest::Types::EnumParam,
                                                    FppTest::Types::ArrayParam,
                                                    FppTest::Types::StructParam>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, ComponentCommandTest, CommandTestImplementations);
// Event tests
using EventTestImplementations = ::testing::Types<FppTest::Types::NoParams,
                                                  FppTest::Types::PrimitiveParams,
                                                  FppTest::Types::LogStringParams,
                                                  FppTest::Types::EnumParam,
                                                  FppTest::Types::ArrayParam,
                                                  FppTest::Types::StructParam,
                                                  FppTest::Types::BoolParam>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, ComponentEventTest, EventTestImplementations);

// Telemetry tests
using TelemetryTestImplementations = ::testing::Types<FppTest::Types::U32Param,
                                                      FppTest::Types::F32Param,
                                                      FppTest::Types::TlmStringParam,
                                                      FppTest::Types::EnumParam,
                                                      FppTest::Types::ArrayParam,
                                                      FppTest::Types::StructParam>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, ComponentTelemetryTest, TelemetryTestImplementations);

// Parameter tests
TEST(ComponentParameterTest, ParameterTest) {
    Tester tester;

    tester.setPrmValid(Fw::ParamValid::VALID);
    tester.testParam();

    tester.setPrmValid(Fw::ParamValid::INVALID);
    tester.testParam();
}

// Parameter tests
using ParamCommandTestImplementations = ::testing::Types<FppTest::Types::BoolParam,
                                                         FppTest::Types::U32Param,
                                                         FppTest::Types::PrmStringParam,
                                                         FppTest::Types::EnumParam,
                                                         FppTest::Types::ArrayParam,
                                                         FppTest::Types::StructParam>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, ComponentParamCommandTest, ParamCommandTestImplementations);

// Time tests
TEST(ComponentTimeTest, TimeTest) {
    Tester tester;
    tester.testTime();
}
