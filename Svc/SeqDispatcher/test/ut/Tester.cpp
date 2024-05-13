// ======================================================================
// \title  SeqDispatcher.hpp
// \author zimri.leisher
// \brief  cpp file for SeqDispatcher test harness implementation class
// ======================================================================

#include "Tester.hpp"

namespace components {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester()
    : SeqDispatcherGTestBase("Tester", Tester::MAX_HISTORY_SIZE),
      component("SeqDispatcher") {
  this->initComponents();
  this->connectPorts();
}

Tester ::~Tester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::testDispatch() {
  // test that it fails when we dispatch too many sequences
  for (int i = 0; i < types::CMD_SEQUENCERS_COUNT; i++) {
    sendCmd_RUN(0, 0, Fw::String("test"), Svc::CmdSequencer_BlockState::BLOCK);
    this->component.doDispatch();
    // no response cuz blocking
    ASSERT_CMD_RESPONSE_SIZE(0);
    ASSERT_EVENTS_SIZE(0);
  }
  ASSERT_TLM_sequencersAvailable(types::CMD_SEQUENCERS_COUNT - 1, 0);
  this->clearHistory();
  // all sequencers should be busy
  sendCmd_RUN(0, 0, Fw::String("test"), Svc::CmdSequencer_BlockState::BLOCK);
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
  sendCmd_RUN(0, 0, Fw::String("test"), Svc::CmdSequencer_BlockState::NO_BLOCK);
  this->component.doDispatch();

  // should immediately return
  ASSERT_EVENTS_SIZE(0);
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN, 0, Fw::CmdResponse::OK);
  this->clearHistory();

  // ok now check that if a sequence errors on block i twill return error
  this->invoke_to_seqDoneIn(1, 0, 0, Fw::CmdResponse::EXECUTION_ERROR);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(0);
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);
  
}

void Tester::testLogStatus() {
  this->sendCmd_RUN(0,0, Fw::String("test"), Svc::CmdSequencer_BlockState::BLOCK);
  this->component.doDispatch();
  this->sendCmd_LOG_STATUS(0,0);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(types::CMD_SEQUENCERS_COUNT);
  ASSERT_EVENTS_LOG_SEQUENCER_STATUS(0, 0, types::CmdSequencerState::RUNNING_SEQUENCE_BLOCK, "test");
}

void Tester::from_seqRunOut_handler(const NATIVE_INT_TYPE portNum,
                                    Fw::String& fileName) {}
void Tester::textLogIn(const FwEventIdType id,         /*!< The event ID*/
                       Fw::Time& timeTag,              /*!< The time*/
                       const Fw::LogSeverity severity, /*!< The severity*/
                       const Fw::TextLogString& text   /*!< The event string*/
) {
  std::cout << text.toChar() << std::endl;
}

}  // end namespace components
