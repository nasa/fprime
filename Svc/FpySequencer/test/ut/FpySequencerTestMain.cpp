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

TEST(Test, Goto) {
  Svc::FpySequencerTester tester;
  tester.test_goto();
}

TEST(Test, If) {
  Svc::FpySequencerTester tester;
  tester.test_if();
}

TEST(Test, SetLVar) {
  Svc::FpySequencerTester tester;
  tester.test_setLvar();
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

TEST(Test, CmdDEBUG_SET_BREAKPOINT) {
  Svc::FpySequencerTester tester;
  tester.test_cmd_DEBUG_SET_BREAKPOINT();
}

TEST(Test, CmdDEBUG_CLEAR_BREAKPOINT) {
  Svc::FpySequencerTester tester;
  tester.test_cmd_DEBUG_CLEAR_BREAKPOINT();
}

TEST(Test, CmdDEBUG_CONTINUE) {
  Svc::FpySequencerTester tester;
  tester.test_cmd_DEBUG_CONTINUE();
}

TEST(Test, CmdDEBUG_BREAK) {
  Svc::FpySequencerTester tester;
  tester.test_cmd_DEBUG_BREAK();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
