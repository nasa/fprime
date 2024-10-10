#include "gtest/gtest.h"

extern "C" {
  void intervalTimerTest();
  void validateFileTest(const char* filename);
  void mutexBasicLockableTest();
}
const char* filename;
// The interval timer unit test is timed off a 1 sec thread delay. Mac OS allows a large amount of
// scheduling jitter to conserve energy, which rarely causes this sleep to be slightly shorter
// (~0.99 s) or longer (~10 sec) than requested, causing the test to fail. The interval timer should
// be rewritten to not directly utilize the OS clock, but in the mean time disabling this test on
// Mac OS prevents intermittent unit test failures.
TEST(Nominal, DISABLED_IntervalTimerTest) {
   intervalTimerTest();
}
TEST(Nominal, ValidateFileTest) {
   validateFileTest(filename);
}

TEST(Nominal, MutexBasicLockableTest) {
  mutexBasicLockableTest();
}

int main(int argc, char* argv[]) {
    filename = argv[0];
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
