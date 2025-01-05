// ======================================================================
// \title  ActiveTextLoggerTestMain.cpp
// \author AlesKus
// \brief  cpp file for ActiveTextLogger component test main function
// ======================================================================

#include "ActiveTextLoggerTester.hpp"


TEST(Nominal, Logging) {
    Svc::ActiveTextLoggerTester tester;
    tester.run_nominal_test();
}

TEST(OffNominal, FileHandling) {
    Svc::ActiveTextLoggerTester tester;
    tester.run_off_nominal_test();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
