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

TEST(Test, shellCommandSucceed) {
  Svc::FileManagerTester tester;
  tester.shellCommandSucceed();
}

TEST(Test, shellCommandFail) {
  Svc::FileManagerTester tester;
  tester.shellCommandFail();
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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
