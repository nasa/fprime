// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include <STest/Random/Random.hpp>

#include "PosixTimeTester.hpp"

TEST(Test, GetTime) {
    Svc::PosixTimeTester tester("Tester");
    tester.getTime();
}

TEST(Test, GetTimeWithContext) {
    Svc::PosixTimeTester tester("Tester");
    tester.getTimeWithContext();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
