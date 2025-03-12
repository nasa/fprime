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
  static const NATIVE_INT_TYPE MAX_HISTORY_SIZE = 10;
  // Instance ID supplied to the component instance under test
  static const NATIVE_INT_TYPE TEST_INSTANCE_ID = 0;
  // Queue depth supplied to component instance under test
  static const NATIVE_INT_TYPE TEST_INSTANCE_QUEUE_DEPTH = 10;

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

  void test_waitRel();
  void test_waitAbs();

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
