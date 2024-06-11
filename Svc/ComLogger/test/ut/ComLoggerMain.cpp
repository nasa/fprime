// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include <iostream>

#include "ComLoggerTester.hpp"

TEST(Test, testLogging) {
  Svc::ComLoggerTester tester("Tester");
  tester.testLogging();
}

TEST(Test, testLoggingNoLength) {
  Svc::ComLoggerTester tester("Tester");
  tester.testLoggingNoLength();
}

TEST(Test, openError) {
  Svc::ComLoggerTester tester("Tester");
  tester.openError();
}

TEST(Test, writeError) {
  Svc::ComLoggerTester tester("Tester");
  tester.writeError();
}

TEST(Test, closeFileCommand) {
  Svc::ComLoggerTester tester("Tester");
  tester.closeFileCommand();
}

TEST(Test, testLoggingWithInit) {
  Svc::ComLoggerTester tester("Tester", true);
  tester.testLoggingWithInit();
}

TEST(Test, noInitError) {
  Svc::ComLoggerTester tester("Tester", true);
  tester.noInitError();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
