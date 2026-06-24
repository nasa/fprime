// ======================================================================
// \title  LedTestMain.cpp
// \author ortega
// \brief  cpp file for Led component test main function
// ======================================================================

#include "LedTester.hpp"

TEST(Nominal, TestBlinking) {
    LedBlinker::LedTester tester;
    tester.testBlinking();
}

TEST(Nominal, TestBlinkInterval) {
    LedBlinker::LedTester tester;
    tester.testBlinkInterval();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
