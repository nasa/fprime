// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include "FppTest/types/FormalParamTypes.hpp"
#include "FppTest/typed_tests/ComponentTest.hpp"

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
