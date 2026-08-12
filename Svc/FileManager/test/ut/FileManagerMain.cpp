// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "FileManagerTester.hpp"

TEST(Test, createDirectorySucceed) {
    Svc::FileManagerTester tester;
    tester.createDirectorySucceed();
}

TEST(Test, createDirectoryFail) {
    Svc::FileManagerTester tester;
    tester.createDirectoryFail();
}

TEST(Test, moveFileSucceed) {
    Svc::FileManagerTester tester;
    tester.moveFileSucceed();
}

TEST(Test, moveFileFail) {
    Svc::FileManagerTester tester;
    tester.moveFileFail();
}

TEST(Test, removeDirectorySucceed) {
    Svc::FileManagerTester tester;
    tester.removeDirectorySucceed();
}

TEST(Test, removeDirectoryFail) {
    Svc::FileManagerTester tester;
    tester.removeDirectoryFail();
}

TEST(Test, removeFileSucceed) {
    Svc::FileManagerTester tester;
    tester.removeFileSucceed();
}

TEST(Test, removeFileFail) {
    Svc::FileManagerTester tester;
    tester.removeFileFail();
}

TEST(Test, appendFileSucceedNewFile) {
    Svc::FileManagerTester tester;
    tester.appendFileSucceed_newFile();
}

TEST(Test, appendFileSucceedExistingFile) {
    Svc::FileManagerTester tester;
    tester.appendFileSucceed_existingFile();
}

TEST(Test, appendFileFail) {
    Svc::FileManagerTester tester;
    tester.appendFileFail();
}

TEST(Test, fileSizeSucceed) {
    Svc::FileManagerTester tester;
    tester.fileSizeSucceed();
}

TEST(Test, fileSizeFail) {
    Svc::FileManagerTester tester;
    tester.fileSizeFail();
}

TEST(Test, calculateCrcSucceed) {
    Svc::FileManagerTester tester;
    tester.calculateCrcSucceed();
}

TEST(Test, listDirectorySucceed) {
    Svc::FileManagerTester tester;
    tester.listDirectorySucceed();
}

TEST(Test, listDirectoryFail) {
    Svc::FileManagerTester tester;
    tester.listDirectoryFail();
}

TEST(Test, listDirectoryWithSubdirs) {
    Svc::FileManagerTester tester;
    tester.listDirectoryWithSubdirs();
}

TEST(Test, generateDpSucceed) {
    Svc::FileManagerTester tester;
    tester.generateDpSucceed();
}

TEST(Test, generateDpFileNotFound) {
    Svc::FileManagerTester tester;
    tester.generateDpFileNotFound();
}

TEST(Test, generateDpEmptyFile) {
    Svc::FileManagerTester tester;
    tester.generateDpEmptyFile();
}

TEST(Test, generateDpChunkSizeClamped) {
    Svc::FileManagerTester tester;
    tester.generateDpChunkSizeClamped();
}

TEST(Test, generateDpPartialRange) {
    Svc::FileManagerTester tester;
    tester.generateDpPartialRange();
}

TEST(Test, generateDpInvalidRange) {
    Svc::FileManagerTester tester;
    tester.generateDpInvalidRange();
}

TEST(Test, generateDpBufferFailure) {
    Svc::FileManagerTester tester;
    tester.generateDpBufferFailure();
}

TEST(Test, generateDpWhileBusy) {
    Svc::FileManagerTester tester;
    tester.generateDpWhileBusy();
}

TEST(Test, generateDpSerializationFailure) {
    Svc::FileManagerTester tester;
    tester.generateDpSerializationFailure();
}

TEST(Test, generateDpImmediateMode) {
    Svc::FileManagerTester tester;
    tester.generateDpImmediateMode();
}

TEST(Test, generateDpCustomPriority) {
    Svc::FileManagerTester tester;
    tester.generateDpCustomPriority();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
