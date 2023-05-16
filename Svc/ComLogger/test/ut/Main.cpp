// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include <iostream>

#include "Tester.hpp"

TEST(Test, testLogging) {
  Svc::Tester tester("Tester");
  tester.testLogging();
}

TEST(Test, testLoggingNoLength) {
  Svc::Tester tester("Tester");
  tester.testLoggingNoLength();
}

TEST(Test, openError) {
  Svc::Tester tester("Tester");
  tester.openError();
}

TEST(Test, writeError) {
  Svc::Tester tester("Tester");
  tester.writeError();
}

TEST(Test, closeFileCommand) {
  Svc::Tester tester("Tester");
  tester.closeFileCommand();
}

TEST(Test, testLoggingWithInit) {
  Svc::Tester tester("Tester", true);
  tester.testLoggingWithInit();
}

TEST(Test, noInitError) {
  Svc::Tester tester("Tester", true);
  tester.noInitError();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
