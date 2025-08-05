// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include <Fw/Test/UnitTest.hpp>
#include "LinuxTimerTester.hpp"

TEST(Nominal, InitTest) {
    TEST_CASE(103.1.1, "Cycle Test");
    Svc::LinuxTimerTester tester;
    tester.runCycles();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
