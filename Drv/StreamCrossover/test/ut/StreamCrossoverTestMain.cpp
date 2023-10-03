// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "StreamCrossoverTester.hpp"

TEST(Nominal, TestBuffer) {
    Drv::StreamCrossoverTester tester;
    tester.sendTestBuffer();
}

TEST(Nominal, TestFail) {
    Drv::StreamCrossoverTester tester;
    tester.testFail();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
