// ======================================================================
// \title  ActiveTextLoggerTestMain.cpp
// \author AlesKus
// \brief  cpp file for ActiveTextLogger component test main function
// ======================================================================

#include "ActiveTextLoggerTester.hpp"


TEST(Nominal, Logging) {
    Svc::ActiveTextLoggerTester tester;
    tester.runNominalTest();
}

TEST(Nominal, WorkstationTime) {
    Svc::ActiveTextLoggerTester tester;
    tester.testWorkstationTimestamp();
}

TEST(OffNominal, FileHandling) {
    Svc::ActiveTextLoggerTester tester;
    tester.runOffNominalTest();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
