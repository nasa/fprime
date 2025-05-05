// ======================================================================
// \title  FpySequencer/test/ut/Tester.hpp
// \author zimri.leisher
// \brief  hpp file for FpySequencer test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "FpySequencerGTestBase.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"

namespace Svc{

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
};

}  // end namespace components

#endif
