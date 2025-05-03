// ======================================================================
// \title  FpySequencer/test/ut/Tester.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer test harness implementation class
// ======================================================================

#ifndef FPYSEQUENCER_TESTER_HPP
#define FPYSEQUENCER_TESTER_HPP

#include "FpySequencerGTestBase.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"

namespace Svc {

class FpySequencerTester : public FpySequencerGTestBase {
  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

 public:
  // Maximum size of histories storing events, telemetry, and port outputs
  static const FwSizeType MAX_HISTORY_SIZE = 10;
  // Instance ID supplied to the component instance under test
  static const FwSizeType TEST_INSTANCE_ID = 0;
  // Queue depth supplied to component instance under test
  static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

  //! Construct object FpySequencerTester
  //!
  FpySequencerTester();

  //! Destroy object FpySequencerTester
  //!
  ~FpySequencerTester();

 public:
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void test_waitRel();
  void test_waitAbs();
  void test_goto();
  void test_setLvar();
  void test_if();
  void test_noOp();

  void test_checkShouldWake();
  void test_checkShouldWakeMismatchBase();
  void test_checkShouldWakeMismatchContext();

  void test_checkStatementTimeout();
  void test_checkStatementTimeoutMismatchBase();
  void test_checkStatementTimeoutMismatchContext();
  
  void test_cmd_RUN();
  void test_cmd_VALIDATE();
  void test_cmd_RUN_VALIDATED();
  void test_cmd_CANCEL();
  void test_cmd_DEBUG_CLEAR_BREAKPOINT();
  void test_cmd_DEBUG_SET_BREAKPOINT();
  void test_cmd_DEBUG_BREAK();
  void test_cmd_DEBUG_CONTINUE();

  void test_readHeader();
  void test_readBody();
  void test_readFooter();
  void test_readBytes();
  void test_validate();

  void test_dispatchStatement();
  void test_dispatchCommand();
  void test_deserialize_waitRel();
  void test_deserialize_waitAbs();
  void test_deserialize_setLVar();
  void test_deserialize_goto();
  void test_deserialize_if();
  void test_deserialize_noOp();

 private:
  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

 private:
  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  //! Connect ports
  //!
  void connectPorts();

  //! Initialize components
  //!
  void initComponents();

 private:
  // ----------------------------------------------------------------------
  // Variables
  // ----------------------------------------------------------------------

  //! The component under test
  //!
  FpySequencer component;

  // dispatches events from the queue until the component reaches the given state
  void dispatchUntilState(State state, U32 bound=100);
  void resetRuntime();
  void assertQueueMsg(FwEnumStoreType msg);

  // a sequence that you can build with the following functions
  Fpy::Sequence seq;

  // clears the sequence we're currently building
  void clearSeq();
  // writes the sequence we're building to a file with the given name
  // writes up to maxBytes bytes
  void writeToFile(const char* name, FwSizeType maxBytes=Fpy::Sequence::SERIALIZED_SIZE);
  void removeFile(const char* name);
  void addStmt(const Fpy::Statement& stmt);
  void addCmd(FwOpcodeType opcode);
  void addDirective(Fpy::DirectiveId id, Fw::StatementArgBuffer& buf);

  void add_WAIT_REL(FpySequencer_WaitRelDirective dir);
  void add_WAIT_ABS(FpySequencer_WaitAbsDirective dir);
  void add_GOTO(FpySequencer_GotoDirective dir);
  void add_SET_LVAR(FpySequencer_SetLocalVarDirective dir);
  void add_IF(FpySequencer_IfDirective dir);
  void add_NO_OP();
};

}  // end namespace components

#endif
