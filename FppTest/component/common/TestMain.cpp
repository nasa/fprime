// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "test/ut/Tester.hpp"
#include "FppTest/component/macros.hpp"
#include "FppTest/typed_tests/ComponentTest.hpp"
#include "FppTest/typed_tests/PortTest.hpp"
#include "FppTest/typed_tests/StringTest.hpp"
#include "FppTest/types/FormalParamTypes.hpp"
#include "FppTest/types/StringArgsPortAc.hpp"

#include "STest/Random/Random.hpp"
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

using SpecialPortTestImplementations = ::testing::Types<
  FppTest::Types::NoParams
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                             ComponentSpecialPortTest,
                             SpecialPortTestImplementations);

using EventTestImplementations = ::testing::Types<
  FppTest::Types::NoParams,
  FppTest::Types::PrimitiveParams,
  FppTest::Types::LogStringParams,
  FppTest::Types::EnumParam,
  FppTest::Types::ArrayParam,
  FppTest::Types::StructParam,
  FppTest::Types::BoolParam
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                             ComponentEventTest,
                             EventTestImplementations);

using TelemetryTestImplementations = ::testing::Types<
  FppTest::Types::U32Param,
  FppTest::Types::F32Param,
  FppTest::Types::TlmStringParam,
  FppTest::Types::EnumParam,
  FppTest::Types::ArrayParam,
  FppTest::Types::StructParam
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                             ComponentTelemetryTest,
                             TelemetryTestImplementations);

TEST(ComponentParameterTest, ParameterTest) {
  Tester tester;
  tester.testParam();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
