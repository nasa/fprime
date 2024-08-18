// ======================================================================
// \title  SeqDispatcher.hpp
// \author zimri.leisher
// \brief  cpp file for SeqDispatcher test harness implementation class
// ======================================================================

#include "SeqDispatcherTester.hpp"

namespace Svc{

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SeqDispatcherTester ::SeqDispatcherTester()
    : SeqDispatcherGTestBase("SeqDispatcherTester", SeqDispatcherTester::MAX_HISTORY_SIZE),
      component("SeqDispatcher") {
  this->connectPorts();
  this->initComponents();
}

SeqDispatcherTester ::~SeqDispatcherTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SeqDispatcherTester ::testDispatch() {
  // test that it fails when we dispatch too many sequences
  for (int i = 0; i < SeqDispatcherSequencerPorts; i++) {
    sendCmd_RUN(0, 0, Fw::String("test"), Fw::Wait::WAIT);
    this->component.doDispatch();
    // no response cuz blocking
    ASSERT_CMD_RESPONSE_SIZE(0);
    ASSERT_EVENTS_SIZE(0);
  }
  ASSERT_TLM_sequencersAvailable(SeqDispatcherSequencerPorts - 1, 0);
  this->clearHistory();
  // all sequencers should be busy
  sendCmd_RUN(0, 0, Fw::String("test"), Fw::Wait::WAIT);
  this->component.doDispatch();
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN, 0,
                      Fw::CmdResponse::EXECUTION_ERROR);

  this->clearHistory();

  this->invoke_to_seqDoneIn(0, 0, 0, Fw::CmdResponse::OK);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(0);
  // we should have gotten a cmd response now
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN, 0, Fw::CmdResponse::OK);

  this->clearHistory();
  // ok now we should be able to send another sequence
  // let's test non blocking now
  sendCmd_RUN(0, 0, Fw::String("test"), Fw::Wait::NO_WAIT);
  this->component.doDispatch();

  // should immediately return
  ASSERT_EVENTS_SIZE(0);
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN, 0, Fw::CmdResponse::OK);
  this->clearHistory();

  // ok now check that if a sequence errors on block it will return error
  this->invoke_to_seqDoneIn(1, 0, 0, Fw::CmdResponse::EXECUTION_ERROR);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(0);
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
  
}

void SeqDispatcherTester::testLogStatus() {
  this->sendCmd_RUN(0,0, Fw::String("test"), Fw::Wait::WAIT);
  this->component.doDispatch();
  this->sendCmd_LOG_STATUS(0,0);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(SeqDispatcherSequencerPorts);
  ASSERT_EVENTS_LogSequencerStatus(0, 0, SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_BLOCK, "test");
}

void SeqDispatcherTester::seqRunOut_handler(
      FwIndexType portNum, //!< The port number
      const Fw::StringBase& filename //!< The sequence file
) {
  this->pushFromPortEntry_seqRunOut(filename);
}

}  // end namespace components
