// ======================================================================
// \title  OsTimeTestMain.cpp
// \author kubiak
// \brief  cpp file for OsTime component test main function
// ======================================================================

#include "OsTimeTester.hpp"

TEST(Nominal, ZeroEpochTest) {
    Svc::OsTimeTester tester;
    tester.zeroEpochTest();
}

TEST(Nominal, OffsetEpochTest) {
    Svc::OsTimeTester tester;
    tester.offsetEpochTest();
}

TEST(Nominal, NoEpochTest) {
    Svc::OsTimeTester tester;
    tester.noEpochTest();
}

TEST(Nominal, UpdateEpochTest) {
    Svc::OsTimeTester tester;
    tester.updateEpochTest();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
