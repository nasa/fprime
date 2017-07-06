// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Test, NominalTlm) {
  Svc::Tester tester;
  tester.nominalTlm();
}

TEST(Test, WarningTlm) {
  Svc::Tester tester;
  tester.warningTlm();
}

TEST(Test, FaultTlm) {
  Svc::Tester tester;
  tester.faultTlm();
}

TEST(Test, DisableAllMonitoring) {
  Svc::Tester tester;
  tester.disableAllMonitoring();
}

TEST(Test, DisableOneMonitoring) {
  Svc::Tester tester;
  tester.disableOneMonitoring();
}

TEST(Test, UpdatePingTimeout) {
  Svc::Tester tester;
  tester.updatePingTimeout();
}

TEST(Test, WatchdogCheck) {
  Svc::Tester tester;
  tester.watchdogCheck();
}

TEST(Test, NominalCmd) {
  Svc::Tester tester;
  tester.nominalCmd();
}

TEST(Test, Nominal2CmdsDuringTlm) {
  Svc::Tester tester;
  tester.nominal2CmdsDuringTlm();
}

TEST(Test, Miscellaneous) {
  Svc::Tester tester;
  tester.miscellaneous();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
