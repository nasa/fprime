// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "BufferLoggerTester.hpp"
#include "Errors.hpp"
#include "Logging.hpp"
#include "Health.hpp"

TEST(Test, LogNoInit) {
  Svc::BufferLoggerTester tester(false); // don't call initLog for the user
  tester.LogNoInit();
}

// ----------------------------------------------------------------------
// Test Errors
// ----------------------------------------------------------------------

TEST(TestErrors, LogFileOpen) {
  Svc::Errors::BufferLoggerTester tester;
  tester.LogFileOpen();
}

TEST(TestErrors, LogFileWrite) {
  Svc::Errors::BufferLoggerTester tester;
  tester.LogFileWrite();
}

TEST(TestErrors, LogFileValidation) {
  Svc::Errors::BufferLoggerTester tester;
  tester.LogFileValidation();
}

// ----------------------------------------------------------------------
// Test Logging
// ----------------------------------------------------------------------

TEST(TestLogging, BufferSendIn) {
  Svc::Logging::BufferLoggerTester tester;
  tester.BufferSendIn();
}

TEST(TestLogging, CloseFile) {
  Svc::Logging::BufferLoggerTester tester;
  tester.CloseFile();
}

TEST(TestLogging, ComIn) {
  Svc::Logging::BufferLoggerTester tester;
  tester.ComIn();
}

TEST(TestLogging, OnOff) {
  Svc::Logging::BufferLoggerTester tester;
  tester.OnOff();
}

// ----------------------------------------------------------------------
// Test Health
// ----------------------------------------------------------------------

TEST(TestHealth, Ping) {
  Svc::Health::BufferLoggerTester tester;
  tester.Ping();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
