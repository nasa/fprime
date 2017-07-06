// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Test, createDirectorySucceed) {
  Svc::Tester tester;
  tester.createDirectorySucceed();
}

TEST(Test, createDirectoryFail) {
  Svc::Tester tester;
  tester.createDirectoryFail();
}

TEST(Test, moveFileSucceed) {
  Svc::Tester tester;
  tester.moveFileSucceed();
}

TEST(Test, moveFileFail) {
  Svc::Tester tester;
  tester.moveFileFail();
}

TEST(Test, removeDirectorySucceed) {
  Svc::Tester tester;
  tester.removeDirectorySucceed();
}

TEST(Test, removeDirectoryFail) {
  Svc::Tester tester;
  tester.removeDirectoryFail();
}

TEST(Test, removeFileSucceed) {
  Svc::Tester tester;
  tester.removeFileSucceed();
}

TEST(Test, removeFileFail) {
  Svc::Tester tester;
  tester.removeFileFail();
}

TEST(Test, shellCommandSucceed) {
  Svc::Tester tester;
  tester.shellCommandSucceed();
}

TEST(Test, shellCommandFail) {
  Svc::Tester tester;
  tester.shellCommandFail();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
