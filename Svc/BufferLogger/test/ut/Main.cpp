// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include "Errors.hpp"
#include "Logging.hpp"
#include "Health.hpp"

TEST(Test, LogNoInit) {
  Svc::Tester tester(false); // don't call initLog for the user
  tester.LogNoInit();
}

// ----------------------------------------------------------------------
// Test Errors
// ----------------------------------------------------------------------

TEST(TestErrors, LogFileOpen) {
  Svc::Errors::Tester tester;
  tester.LogFileOpen();
}

TEST(TestErrors, LogFileWrite) {
  Svc::Errors::Tester tester;
  tester.LogFileWrite();
}

TEST(TestErrors, LogFileValidation) {
  Svc::Errors::Tester tester;
  tester.LogFileValidation();
}

// ----------------------------------------------------------------------
// Test Logging
// ----------------------------------------------------------------------

TEST(TestLogging, BufferSendIn) {
  Svc::Logging::Tester tester;
  tester.BufferSendIn();
}

TEST(TestLogging, CloseFile) {
  Svc::Logging::Tester tester;
  tester.CloseFile();
}

TEST(TestLogging, ComIn) {
  Svc::Logging::Tester tester;
  tester.ComIn();
}

TEST(TestLogging, OnOff) {
  Svc::Logging::Tester tester;
  tester.OnOff();
}

// ----------------------------------------------------------------------
// Test Health
// ----------------------------------------------------------------------

TEST(TestHealth, Ping) {
  Svc::Health::Tester tester;
  tester.Ping();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
