#include "FppTest/typed_tests/PortTest.hpp"
#include "FppTest/types/FormalParamTypes.hpp"

// Typed async port tests
using TypedAsyncPortTestImplementations = ::testing::Types<
    FppTest::Types::NoParams,
    FppTest::Types::PrimitiveParams,
    FppTest::Types::PortStringParams,
    FppTest::Types::EnumParams,
    FppTest::Types::ArrayParams,
    FppTest::Types::StructParams
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                               TypedAsyncPortTest,
                               TypedAsyncPortTestImplementations);

// Serial async port tests
using SerialAsyncPortTestImplementations = ::testing::Types<
    FppTest::Types::NoParams,
    FppTest::Types::PrimitiveParams,
    FppTest::Types::PortStringParams,
    FppTest::Types::EnumParams,
    FppTest::Types::ArrayParams,
    FppTest::Types::StructParams
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                               SerialAsyncPortTest,
                               SerialAsyncPortTestImplementations);
