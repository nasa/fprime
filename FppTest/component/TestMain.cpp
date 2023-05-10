// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "test/ut/Tester.hpp"
#include "FppTest/component/macros.hpp"
#include "FppTest/types/FormalParamTypes.hpp"
#include "FppTest/typed_tests/ComponentTest.hpp"

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
    return RUN_ALL_TESTS();
}
