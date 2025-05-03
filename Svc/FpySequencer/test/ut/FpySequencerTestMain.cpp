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

TEST(Test, NoOp) {
  Svc::FpySequencerTester tester;
  tester.test_noOp();
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

TEST(Test, dispatchStatement) {
  Svc::FpySequencerTester tester;
  tester.test_dispatchStatement();
}

TEST(Test, dispatchCommand) {
  Svc::FpySequencerTester tester;
  tester.test_dispatchCommand();
}

TEST(Test, deserialize_waitRel) {
  Svc::FpySequencerTester tester;
  tester.test_deserialize_waitRel();
}

TEST(Test, deserialize_waitAbs) {
  Svc::FpySequencerTester tester;
  tester.test_deserialize_waitAbs();
}

TEST(Test, deserialize_setLVar) {
  Svc::FpySequencerTester tester;
  tester.test_deserialize_setLVar();
}

TEST(Test, deserialize_goto) {
  Svc::FpySequencerTester tester;
  tester.test_deserialize_goto();
}

TEST(Test, deserialize_if) {
  Svc::FpySequencerTester tester;
  tester.test_deserialize_if();
}

TEST(Test, deserialize_noOp) {
  Svc::FpySequencerTester tester;
  tester.test_deserialize_noOp();
}

TEST(Test, readHeader) {
  Svc::FpySequencerTester tester;
  tester.test_readHeader();
}

TEST(Test, readBody) {
  Svc::FpySequencerTester tester;
  tester.test_readBody();
}

TEST(Test, readFooter) {
  Svc::FpySequencerTester tester;
  tester.test_readFooter();
}

TEST(Test, readBytes) {
  Svc::FpySequencerTester tester;
  tester.test_readBytes();
}

TEST(Test, validate) {
  Svc::FpySequencerTester tester;
  tester.test_validate();
}

TEST(Test, allocateBuffer) {
  Svc::FpySequencerTester tester;
  tester.test_allocateBuffer();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
