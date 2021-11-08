// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, TestMem) {
    Svc::Tester tester;
    tester.testMemRead();
}

TEST(Nominal, TestPhysMem) {
    Svc::Tester tester;
    tester.testPhysMemRead();
}

TEST(Nominal, TestCpuUtilRead) {
    Svc::Tester tester;
    tester.testCpuUtilRead();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
