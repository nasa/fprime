// ======================================================================
// \title  FileWorkerTestMain.cpp
// \author racheljt
// \brief  cpp file for FileWorker component test main function
// ======================================================================

#include "FileWorkerTester.hpp"

TEST(Nominal, testReadFile) {
    Svc::FileWorkerTester tester;
    tester.testReadFile();
}

TEST(Nominal, testCancel) {
    Svc::FileWorkerTester tester;
    tester.testCancel();
}

TEST(Nominal, testFileError) {
    Svc::FileWorkerTester tester;
    tester.testFileError();
}

TEST(Nominal, testVerify) {
    Svc::FileWorkerTester tester;
    tester.testVerify();
}

TEST(Nominal, testTransfer) {
    Svc::FileWorkerTester tester;
    tester.testTransfer();
}

TEST(Nominal, testWriting) {
    Svc::FileWorkerTester tester;
    tester.testWriting();
}

TEST(Nominal, testWritingOffset) {
    Svc::FileWorkerTester tester;
    tester.testWritingOffset();
}

TEST(Nominal, testWriteZeroLength) {
    Svc::FileWorkerTester tester;
    tester.testWriteZeroLength();
}

TEST(Nominal, testAppending) {
    Svc::FileWorkerTester tester;
    tester.testAppending();
}

TEST(Nominal, testWriteReadRoundTrip) {
    Svc::FileWorkerTester tester;
    tester.testWriteReadRoundTrip();
}

TEST(Nominal, testTimeout) {
    Svc::FileWorkerTester tester;
    tester.testTimeout();
}

TEST(Nominal, testPathTooLong) {
    Svc::FileWorkerTester tester;
    tester.testPathTooLong();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
