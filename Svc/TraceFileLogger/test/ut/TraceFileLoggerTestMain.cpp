// ======================================================================
// \title  TraceLoggerTestMain.cpp
// \author sreddy
// \brief  cpp file for TraceLogger component test main function
// ======================================================================

#include "TraceFileLoggerTester.hpp"

TEST(Nominal, test_startup) {
    Svc::TraceLoggerTester tester;
    tester.test_startup();
}

TEST(Nominal, test_log_file) {
    Svc::TraceLoggerTester tester;
    tester.test_startup();
    tester.test_file();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
