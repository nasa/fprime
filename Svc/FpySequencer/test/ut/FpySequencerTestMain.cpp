// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "FpySequencerTester.hpp"

TEST(Test, WaitRel) {
  Svc::FpySequencerTester tester;
  tester.test_waitRel();
}

TEST(Test, WaitAbs) {
  Svc::FpySequencerTester tester;
  tester.test_waitAbs();
}

TEST(Test, CheckShouldWake) {
  Svc::FpySequencerTester tester;
  tester.test_checkShouldWake();
}

TEST(Test, CheckShouldWakeBadContext) {
  Svc::FpySequencerTester tester;
  tester.test_checkShouldWakeMismatchContext();
}

TEST(Test, CheckShouldWakeBadBase) {
  Svc::FpySequencerTester tester;
  tester.test_checkShouldWakeMismatchBase();
}

TEST(Test, CheckStatementTimeout) {
  Svc::FpySequencerTester tester;
  tester.test_checkStatementTimeout();
}

TEST(Test, CheckStatementTimeoutMismatchContext) {
  Svc::FpySequencerTester tester;
  tester.test_checkStatementTimeoutMismatchContext();
}

TEST(Test, CheckStatementTimeoutMismatchBase) {
  Svc::FpySequencerTester tester;
  tester.test_checkStatementTimeoutMismatchBase();
}

TEST(Test, CmdRUN) {
  Svc::FpySequencerTester tester;
  tester.test_cmd_RUN();
}

TEST(Test, CmdVALIDATE) {
  Svc::FpySequencerTester tester;
  tester.test_cmd_VALIDATE();
}

TEST(Test, CmdRUN_VALIDATED) {
  Svc::FpySequencerTester tester;
  tester.test_cmd_RUN_VALIDATED();
}

TEST(Test, CmdCANCEL) {
  Svc::FpySequencerTester tester;
  tester.test_cmd_CANCEL();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
