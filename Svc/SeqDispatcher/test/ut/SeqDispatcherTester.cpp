// ======================================================================
// \title  SeqDispatcher.hpp
// \author zimri.leisher
// \brief  cpp file for SeqDispatcher test harness implementation class
// ======================================================================

#include "SeqDispatcherTester.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SeqDispatcherTester ::SeqDispatcherTester()
    : SeqDispatcherGTestBase("SeqDispatcherTester", SeqDispatcherTester::MAX_HISTORY_SIZE), component("SeqDispatcher") {
    this->connectPorts();
    this->initComponents();
}

SeqDispatcherTester ::~SeqDispatcherTester() {
    this->component.deinit();
}

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
    ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN, 0, Fw::CmdResponse::EXECUTION_ERROR);

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
    this->sendCmd_RUN(0, 0, Fw::String("test"), Fw::Wait::WAIT);
    this->component.doDispatch();
    this->clearHistory();
    this->sendCmd_LOG_STATUS(0, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_SIZE(SeqDispatcherSequencerPorts);
    ASSERT_EVENTS_LogSequencerStatus(0, 0, SeqDispatcher_CmdSequencerState::RUNNING_SEQUENCE_BLOCK, "test");
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_LOG_STATUS, 0, Fw::CmdResponse::OK);
}

void SeqDispatcherTester::seqRunOut_handler(FwIndexType portNum,             //!< The port number
                                            const Fw::StringBase& filename,  //!< The sequence file
                                            const Svc::SeqArgs& args         //!< Sequence arguments
) {
    this->pushFromPortEntry_seqRunOut(filename, args);
}

// Test RUN_ARGS with valid arguments - verify arguments are propagated correctly
void SeqDispatcherTester::testRunArgsWithValidArguments() {
    // Create test arguments with some data
    // Note: Keep size small to fit within FW_CMD_ARG_BUFFER_MAX_SIZE constraints
    // Total command payload must fit: filename (~44 bytes) + Fw::Wait (4 bytes) + SeqArgs
    // With FW_CMD_ARG_BUFFER_MAX_SIZE ~= 500 bytes, SeqArgs should be < 400 bytes total
    Svc::SeqArgs testArgs{0, 0};

    // Send RUN_ARGS command with non-blocking mode
    sendCmd_RUN_ARGS(0, 0, Fw::String("test"), Fw::Wait::NO_WAIT, testArgs);
    this->component.doDispatch();

    // Should get immediate response for non-blocking
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN_ARGS, 0, Fw::CmdResponse::OK);

    // Verify that seqRunOut was called with correct arguments
    ASSERT_from_seqRunOut_SIZE(1);
    ASSERT_from_seqRunOut(0, Fw::String("test"), testArgs);

    // Verify telemetry
    ASSERT_TLM_dispatchedCount(0, 1);
    ASSERT_TLM_sequencersAvailable(0, SeqDispatcherSequencerPorts - 1);
}

// Test RUN_ARGS with maximum-sized arguments - test boundary conditions
void SeqDispatcherTester::testRunArgsWithMaxSizedArguments() {
    constexpr FwSizeType TEST_ARG_SIZE = SequenceArgumentsMaxSize;
    Svc::SeqArgs largeArgs(TEST_ARG_SIZE, 0);
    U8* buffer = largeArgs.get_buffer();
    for (FwSizeType i = 0; i < TEST_ARG_SIZE; i++) {
        buffer[i] = static_cast<U8>(i % 256);
    }

    // Send RUN_ARGS command with large arguments (use short filename to save space)
    sendCmd_RUN_ARGS(0, 0, Fw::String("test"), Fw::Wait::NO_WAIT, largeArgs);
    this->component.doDispatch();

    // Should get immediate response for non-blocking
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN_ARGS, 0, Fw::CmdResponse::OK);

    // Verify that seqRunOut was called with correct large arguments
    ASSERT_from_seqRunOut_SIZE(1);
    ASSERT_from_seqRunOut(0, Fw::String("test"), largeArgs);

    // Verify telemetry
    ASSERT_TLM_dispatchedCount(0, 1);
}

// Test RUN_ARGS when no sequencers available - verify error handling
void SeqDispatcherTester::testRunArgsNoSequencersAvailable() {
    // Fill all sequencers
    Svc::SeqArgs emptyArgs{0, 0};
    for (int i = 0; i < SeqDispatcherSequencerPorts; i++) {
        sendCmd_RUN_ARGS(0, 0, Fw::String("test"), Fw::Wait::WAIT, emptyArgs);
        this->component.doDispatch();
        // no response because blocking
        ASSERT_CMD_RESPONSE_SIZE(0);
    }
    ASSERT_TLM_sequencersAvailable(SeqDispatcherSequencerPorts - 1, 0);
    this->clearHistory();

    // Now try to send another sequence when all are busy
    Svc::SeqArgs testArgs;
    testArgs.set_size(0);

    sendCmd_RUN_ARGS(0, 0, Fw::String("test"), Fw::Wait::WAIT, testArgs);
    this->component.doDispatch();

    // Should get error response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN_ARGS, 0, Fw::CmdResponse::EXECUTION_ERROR);

    // Should get warning event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_NoAvailableSequencers_SIZE(1);

    // Verify no seqRunOut was called
    ASSERT_from_seqRunOut_SIZE(0);
}

// Test RUN_ARGS with blocking vs non-blocking behavior
void SeqDispatcherTester::testRunArgsBlockingVsNonBlocking() {
    Svc::SeqArgs testArgs{0, 0};

    // Test non-blocking mode - should get immediate response
    sendCmd_RUN_ARGS(0, 0, Fw::String("nonblocking"), Fw::Wait::NO_WAIT, testArgs);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN_ARGS, 0, Fw::CmdResponse::OK);
    ASSERT_from_seqRunOut_SIZE(1);
    this->clearHistory();

    // Free up the sequencer
    this->invoke_to_seqDoneIn(0, 0, 0, Fw::CmdResponse::OK);
    this->component.doDispatch();
    this->clearHistory();

    // Test blocking mode - should NOT get immediate response
    sendCmd_RUN_ARGS(0, 0, Fw::String("blocking"), Fw::Wait::WAIT, testArgs);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(0);  // No response yet
    ASSERT_from_seqRunOut_SIZE(1);
    ASSERT_from_seqRunOut(0, Fw::String("blocking"), testArgs);

    // Now complete the sequence
    this->invoke_to_seqDoneIn(0, 0, 0, Fw::CmdResponse::OK);
    this->component.doDispatch();

    // Should now get response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN_ARGS, 0, Fw::CmdResponse::OK);

    // Test blocking mode with error response
    this->clearHistory();
    sendCmd_RUN_ARGS(0, 0, Fw::String("blocking_error"), Fw::Wait::WAIT, testArgs);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(0);  // No response yet

    // Complete with error
    this->invoke_to_seqDoneIn(0, 0, 0, Fw::CmdResponse::EXECUTION_ERROR);
    this->component.doDispatch();

    // Should get error response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, SeqDispatcher::OPCODE_RUN_ARGS, 0, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_TLM_errorCount(0, 1);
}

}  // namespace Svc
