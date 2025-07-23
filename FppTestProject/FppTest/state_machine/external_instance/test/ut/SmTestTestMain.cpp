// ----------------------------------------------------------------------
// SmTestTestMain.cpp
// ----------------------------------------------------------------------

#include "FppTest/state_machine/external_instance/test/ut/SmTestTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"

using namespace FppTest;

TEST(schedIn, OK) {
    COMMENT("schedIn OK");
    SmTestTester tester;
    tester.schedIn_OK();
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
