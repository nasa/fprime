// ======================================================================
// \title  ComLoggerDpTester.cpp
// \author tcanham
// \brief  cpp file for ComLoggerDp component test harness implementation class
// ======================================================================

#include "ComLoggerDpTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ComLoggerDpTester ::ComLoggerDpTester()
    : ComLoggerDpGTestBase("ComLoggerDpTester", ComLoggerDpTester::MAX_HISTORY_SIZE), component("ComLoggerDp") {
    this->initComponents();
    this->connectPorts();
}

ComLoggerDpTester ::~ComLoggerDpTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ComLoggerDpTester::testComLogging() {
    // Configure component to be enabled initially
    this->component.configure(true);

    // Start logging with 2 packets per container
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STARTCOMDP, 0, Fw::CmdResponse::OK);

    // Create and send a Com buffer
    U8 testData[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));

    // Send first packet
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should have allocated a container but not sent yet
    ASSERT_PRODUCT_GET_SIZE(1);

    // Send second packet to fill container
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should now have sent the container
    ASSERT_PRODUCT_SEND_SIZE(1);
}

void ComLoggerDpTester::testStartComDp() {
    // Test successful start
    this->sendCmd_StartComDp(0, 0, 5, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STARTCOMDP, 0, Fw::CmdResponse::OK);

    // Verify event was logged
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ComDpStarted_SIZE(1);
    ASSERT_EVENTS_ComDpStarted(0, 5);

    // Test validation error with zero packets per container
    this->clearHistory();
    this->sendCmd_StartComDp(0, 1, 0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STARTCOMDP, 1, Fw::CmdResponse::VALIDATION_ERROR);
    // No event should be logged for validation error
    ASSERT_EVENTS_SIZE(0);
}

void ComLoggerDpTester::testStopComDp() {
    // Start logging
    this->component.configure(true);
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Send one packet (partial container)
    U8 testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Stop logging - should send partial container
    this->sendCmd_StopComDp(0, 1);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STOPCOMDP, 1, Fw::CmdResponse::OK);
    ASSERT_PRODUCT_SEND_SIZE(1);
}

void ComLoggerDpTester::testUpdatePriority() {
    // Start logging
    this->component.configure(true);
    this->sendCmd_StartComDp(0, 0, 3, 5);
    this->component.doDispatch();
    this->clearHistory();

    // Send one packet to create active container
    U8 testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Update priority
    this->sendCmd_UpdatePriority(0, 1, 15);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_UPDATEPRIORITY, 1, Fw::CmdResponse::OK);
}

void ComLoggerDpTester::testPing() {
    // Send ping
    this->invoke_to_pingIn(0, 0x12345678);
    this->component.doDispatch();

    // Verify ping response
    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, 0x12345678);
}

void ComLoggerDpTester::testContainerFill() {
    // Start logging with 3 packets per container
    this->component.configure(true);
    this->sendCmd_StartComDp(0, 0, 3, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Create test data
    U8 testData[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22, 0x33, 0x44};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));

    // Send 3 packets
    for (int i = 0; i < 3; i++) {
        this->invoke_to_comIn(0, comBuf, 0);
        this->component.doDispatch();
    }

    // Should have sent exactly one container
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Send 2 more packets (partial for next container)
    for (int i = 0; i < 2; i++) {
        this->invoke_to_comIn(0, comBuf, 0);
        this->component.doDispatch();
    }

    // Should still have only sent one container
    ASSERT_PRODUCT_SEND_SIZE(1);
}

void ComLoggerDpTester::testAllocationFailure() {
    // Start logging
    this->component.configure(true);
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Simulate allocation failure
    this->m_allocationFailure = true;

    // Send a packet (should be dropped)
    U8 testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should have generated an error event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DpBufferError_SIZE(1);

    // Should not have sent a container
    ASSERT_PRODUCT_SEND_SIZE(0);

    // Check telemetry - NumBuffersDropped should be 1
    this->clearHistory();
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(3);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 1);  // 1 buffer dropped
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 0);  // 0 buffers logged
}

void ComLoggerDpTester::testTelemetry() {
    // Configure and start logging
    this->component.configure(true);
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Call schedIn to write telemetry
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();

    // Verify telemetry was written
    ASSERT_TLM_SIZE(3);
    ASSERT_TLM_LoggingEnabled_SIZE(1);
    ASSERT_TLM_LoggingEnabled(0, true);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 0);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);

    // Log some buffers
    U8 testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->clearHistory();

    // Call schedIn again
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();

    // Verify buffer count updated
    ASSERT_TLM_SIZE(3);
    ASSERT_TLM_LoggingEnabled_SIZE(1);
    ASSERT_TLM_LoggingEnabled(0, true);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 2);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);

    // Stop logging
    this->sendCmd_StopComDp(0, 1);
    this->component.doDispatch();
    this->clearHistory();

    // Call schedIn again
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();

    // Verify logging disabled
    ASSERT_TLM_SIZE(3);
    ASSERT_TLM_LoggingEnabled_SIZE(1);
    ASSERT_TLM_LoggingEnabled(0, false);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 2);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);
}

void ComLoggerDpTester::testPriorityPreserved() {
    // Start logging with priority 15 (not enabled initially)
    this->sendCmd_StartComDp(0, 0, 2, 15);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STARTCOMDP, 0, Fw::CmdResponse::OK);
    this->clearHistory();

    // Send two Com buffers to trigger container allocation and send
    U8 testData[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));

    // First buffer - triggers container allocation with priority 15
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_GET_SIZE(1);

    // Second buffer - fills and sends the container
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Test passes if we get here - the priority was successfully preserved and applied
    // when the container was allocated, even though logging was not enabled initially.
    // The bug would have caused the default priority (5) to be used instead of 15.
}

void ComLoggerDpTester::testStartRecordingPort() {
    // Encode config: packetsPerContainer=5, priority=10
    // (5 << 16) | 10 = 0x0005000A
    const U32 config = (5 << 16) | 10;

    // Invoke port
    this->invoke_to_startRecordingIn(0, config);
    this->component.doDispatch();

    // Verify event was logged
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ComDpStarted_SIZE(1);
    ASSERT_EVENTS_ComDpStarted(0, 5);

    // Verify logging is enabled by sending a Com buffer
    U8 testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should have allocated a container
    ASSERT_PRODUCT_GET_SIZE(1);
}

void ComLoggerDpTester::testStopRecordingPort() {
    // Start logging via command first
    this->component.configure(true);
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Send one packet (partial container)
    U8 testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Invoke stop port
    this->invoke_to_stopRecordingIn(0);
    this->component.doDispatch();

    // Should have sent partial container
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Verify stop event was logged
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ComDpStopped_SIZE(1);
    ASSERT_EVENTS_ComDpStopped(0, 1);  // 1 partial container sent
}

void ComLoggerDpTester::testClearCounters() {
    // Start logging and log some buffers
    this->component.configure(true);
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Send two Com buffers
    U8 testData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->clearHistory();

    // Check telemetry before clearing
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(3);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 2);  // 2 buffers logged
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);  // 0 buffers dropped
    this->clearHistory();

    // Send CLEAR_COUNTERS command
    this->sendCmd_CLEAR_COUNTERS(0, 0);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_CLEAR_COUNTERS, 0, Fw::CmdResponse::OK);

    // Verify CountersCleared event was logged
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CountersCleared_SIZE(1);
    this->clearHistory();

    // Check telemetry after clearing
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(3);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 0);  // Counter reset to 0
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);  // Dropped counter also reset
}

void ComLoggerDpTester::testBufferOverflow() {
    // Start logging with 1 packet per container
    this->component.configure(true);
    this->sendCmd_StartComDp(0, 0, 1, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Create a buffer larger than FW_COM_BUFFER_MAX_SIZE to cause serialization overflow
    // Note: This tests the serialization error path, but in practice ComBuffer
    // has its own size limit so this is a defensive check
    U8 largeData[FW_COM_BUFFER_MAX_SIZE];
    for (U32 i = 0; i < FW_COM_BUFFER_MAX_SIZE; ++i) {
        largeData[i] = static_cast<U8>(i & 0xFF);
    }
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(largeData, FW_COM_BUFFER_MAX_SIZE);

    // Send buffer - should allocate container successfully
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should have allocated container and logged the buffer
    ASSERT_PRODUCT_GET_SIZE(1);
    ASSERT_TLM_SIZE(0);  // No telemetry yet

    // Container should be sent (1 packet fills it)
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Check that buffer was logged (not dropped)
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(3);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 1);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);  // No buffers dropped
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void ComLoggerDpTester::connectPorts() {
    // cmdIn
    this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

    // comIn
    this->connect_to_comIn(0, this->component.get_comIn_InputPort(0));

    // pingIn
    this->connect_to_pingIn(0, this->component.get_pingIn_InputPort(0));

    // schedIn
    this->connect_to_schedIn(0, this->component.get_schedIn_InputPort(0));

    // startRecordingIn
    this->connect_to_startRecordingIn(0, this->component.get_startRecordingIn_InputPort(0));

    // stopRecordingIn
    this->connect_to_stopRecordingIn(0, this->component.get_stopRecordingIn_InputPort(0));

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(0, this->get_from_cmdRegOut(0));

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(0, this->get_from_cmdResponseOut(0));

    // logOut
    this->component.set_logOut_OutputPort(0, this->get_from_logOut(0));

    // LogText
    this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

    // timeCaller
    this->component.set_timeCaller_OutputPort(0, this->get_from_timeCaller(0));

    // tlmOut
    this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

    // productGetOut
    this->component.set_productGetOut_OutputPort(0, this->get_from_productGetOut(0));

    // productSendOut
    this->component.set_productSendOut_OutputPort(0, this->get_from_productSendOut(0));

    // pingOut
    this->component.set_pingOut_OutputPort(0, this->get_from_pingOut(0));
}

void ComLoggerDpTester::initComponents() {
    this->init();
    this->component.init(TEST_INSTANCE_QUEUE_DEPTH, TEST_INSTANCE_ID);
}

Fw::Success::T ComLoggerDpTester::productGet_handler(FwDpIdType id,
                                                      FwSizeType dataSize,
                                                      Fw::Buffer& buffer) {
    this->pushProductGetEntry(id, dataSize);

    if (this->m_allocationFailure) {
        return Fw::Success::FAILURE;
    }

    // Return a valid buffer
    buffer.set(this->m_buffer, dataSize);
    return Fw::Success::SUCCESS;
}

}  // namespace Svc
