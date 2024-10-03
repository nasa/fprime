#include "gtest/gtest.h"
#include <cstdlib>
#include <unistd.h>
#include <cstdio>

extern "C" {
/*  void qtest_block_receive();
  void qtest_nonblock_receive();
  void qtest_nonblock_send();
  void qtest_block_send();
  void qtest_performance();
  void qtest_concurrent();*/
  void intervalTimerTest();
  void validateFileTest(const char* filename);
  void systemResourcesTest();
  void mutexBasicLockableTest();
}
const char* filename;
/*TEST(Nominal, QTestBlockRecv) {
   qtest_block_receive();
}
TEST(Nominal, QTestNonBlockRecv) {
   qtest_nonblock_receive();
}
TEST(Nominal, QTestNonBlockSend) {
   qtest_nonblock_send();
}
TEST(Nominal, QTestBlockSend) {
   qtest_block_send();
}
TEST(Nominal, QTestPerformance) {
   qtest_performance();
}
TEST(Nominal, QTestConcurrentTest) {
   qtest_concurrent();
}
*/
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
TEST(Nominal, SystemResourcesTest) { 
   systemResourcesTest();
}
TEST(Nominal, MutexBasicLockableTest) {
  mutexBasicLockableTest();
}

int main(int argc, char* argv[]) {
    filename = argv[0];
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
