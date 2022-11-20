// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "FppTest/port/NoArgsPortAc.hpp"
#include "FppTest/port/ReturnU32PortAc.hpp"
#include "FppTest/typed_tests/PortTest.hpp"
#include "Tester.hpp"

using PortTestImplementations = ::testing::Types<
    NoArgsPort,
    ReturnU32Port
>;

INSTANTIATE_TYPED_TEST_SUITE_P(FppTest,
                               PortTest,
                               PortTestImplementations);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
