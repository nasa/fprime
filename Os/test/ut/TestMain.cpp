#include "gtest/gtest.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

extern "C" {
  void startTestTask();
  void qtest_block_receive();
  void qtest_nonblock_receive();
  void qtest_nonblock_send();
  void qtest_block_send();
  void qtest_performance();
  void qtest_concurrent();
  void intervalTimerTest();
  void fileSystemTest();
  void validateFileTest(const char* filename);
}
const char* filename;
TEST(Nominal, StartTestTask) {
   startTestTask();
}
TEST(Nominal, QTestBlockRecv) {
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
TEST(Nominal, IntervalTimerTest) {
   intervalTimerTest();
}
TEST(Nominal, FileSystemTest) {
   fileSystemTest();
}
TEST(Nominal, ValidateFileTest) {
   validateFileTest(filename);
}

int main(int argc, char* argv[]) {
    filename = argv[0];
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
