// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "FppTest/port/NoArgsPortAc.hpp"
#include "FppTest/port/PrimitiveArgsPortAc.hpp"
#include "FppTest/port/NoArgsReturnPortAc.hpp"
#include "FppTest/port/PrimitiveReturnPortAc.hpp"

#include "Tester.hpp"
#include "FppTest/port/PortStructs.hpp"
#include "FppTest/typed_tests/PortTest.hpp"

#include "gtest/gtest.h"

using PortTestImplementations = ::testing::Types<
    FppTest::Port::NoArgsPort,
    FppTest::Port::PrimitiveArgsPort,
    FppTest::Port::NoArgsReturnPort,
    FppTest::Port::PrimitiveReturnPort
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                               PortTest,
                               PortTestImplementations);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
