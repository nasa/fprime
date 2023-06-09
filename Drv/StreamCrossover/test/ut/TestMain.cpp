// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, TestBuffer) {
    Drv::Tester tester;
    tester.sendTestBuffer();
}

TEST(Nominal, TestFail) {
    Drv::Tester tester;
    tester.testFail();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
