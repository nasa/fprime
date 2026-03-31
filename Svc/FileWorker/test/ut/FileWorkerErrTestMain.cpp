// ======================================================================
// \title  FileWorkerTestMain.cpp
// \author racheljt
// \brief  cpp file for FileWorker component test main function
// ======================================================================

#include "FileWorkerErrTester.hpp"

TEST(Nominal, testReadErr) {
    Svc::FileWorkerTester tester;
    tester.testReadErr();
}

TEST(Nominal, testVerifyErr) {
    Svc::FileWorkerTester tester;
    tester.testVerifyErr();
}

TEST(Nominal, testWriteErr) {
    Svc::FileWorkerTester tester;
    tester.testWriteErr();
}

TEST(Nominal, testWriteHashErr) {
    Svc::FileWorkerTester tester;
    tester.testWriteHashErr();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
