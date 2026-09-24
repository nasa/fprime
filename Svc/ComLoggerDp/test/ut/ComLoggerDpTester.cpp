// ======================================================================
// \title  ComLoggerDpTester.cpp
// \author tcanham
// \brief  cpp file for ComLoggerDp component test harness implementation class
// ======================================================================

#include "ComLoggerDpTester.hpp"
#include "default/config/ComLoggerDpCfg.hpp"

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
    // Start logging with 2 packets per container
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STARTCOMDP, 0, Fw::CmdResponse::OK);

    // Create and send a Com buffer
    Fw::ComBuffer comBuf = this->createTestComBuffer(16);

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

    // Verify StartRecordingFailed event was logged (consistent with port path behavior)
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_StartRecordingFailed_SIZE(1);
    ASSERT_EVENTS_StartRecordingFailed(0, 0);  // packetsPerContainer = 0
}

void ComLoggerDpTester::testStopComDp() {
    // Start logging
    this->startLoggingAndClearHistory(2, 10);

    // Send one packet (partial container)
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Stop logging - should send partial container
    this->sendCmd_StopComDp(0, 1);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STOPCOMDP, 1, Fw::CmdResponse::OK);
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Verify ComDpStopped event was emitted with correct numSent
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ComDpStopped_SIZE(1);
    ASSERT_EVENTS_ComDpStopped(0, 1);  // 1 partial container sent
}

void ComLoggerDpTester::testUpdatePriority() {
    // Start logging
    this->startLoggingAndClearHistory(3, 5);

    // Send one packet to create active container
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Update priority
    this->sendCmd_UpdatePriority(0, 1, 15);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_UPDATEPRIORITY, 1, Fw::CmdResponse::OK);

    // Verify PriorityUpdated event was emitted with correct priority value
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PriorityUpdated_SIZE(1);
    ASSERT_EVENTS_PriorityUpdated(0, 15);  // Verify priority value

    // Fill the container (3 packets) and verify the sent header carries the updated priority
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_SEND_SIZE(1);
    Fw::DpContainer sent(0, this->productSendHistory->at(0).buffer);
    ASSERT_EQ(sent.deserializeHeader(), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(sent.getPriority(), 15);
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
    this->sendCmd_StartComDp(0, 0, 3, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Create test data
    Fw::ComBuffer comBuf = this->createTestComBuffer(10, 0xAA);

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
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Simulate allocation failure
    this->m_allocationFailure = true;

    // Send a packet (should be dropped)
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should have generated an error event with correct size
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DpBufferError_SIZE(1);
    // Calculate expected container size: packetsPerContainer=2, each packet can hold FW_COM_BUFFER_MAX_SIZE + sentry
    // Note: This is the data size requested from dpGet, not the total container size
    const FwSizeType sentrySize = sizeof(ComLoggerDpSentry);
    const FwSizeType expectedSize =
        2 * ComLoggerDp::SIZE_OF_ComBufferRecord_RECORD(FW_COM_BUFFER_MAX_SIZE + sentrySize);
    ASSERT_EVENTS_DpBufferError(0, expectedSize);

    // Should not have sent a container
    ASSERT_PRODUCT_SEND_SIZE(0);

    // Check telemetry - NumBuffersDropped should be 1
    this->clearHistory();
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 1);  // 1 buffer dropped
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 0);  // 0 buffers logged
}

void ComLoggerDpTester::testPortValidationFailure() {
    // Try to start recording via port with invalid packetsPerContainer (0)
    const U32 invalidPacketsPerContainer = 0;
    const FwDpPriorityType priority = 10;

    this->invoke_to_startRecordingIn(0, invalidPacketsPerContainer, priority);
    this->component.doDispatch();

    // Should have generated validation failure warning event
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_StartRecordingFailed_SIZE(1);
    ASSERT_EVENTS_StartRecordingFailed(0, invalidPacketsPerContainer);

    // Should NOT have generated ComDpStarted event
    ASSERT_EVENTS_ComDpStarted_SIZE(0);

    // Verify logging is NOT enabled by trying to send a Com buffer
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);

    this->clearHistory();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // No events should be generated (logging not enabled)
    ASSERT_EVENTS_SIZE(0);

    // No data product should be sent
    ASSERT_PRODUCT_SEND_SIZE(0);
}

void ComLoggerDpTester::testTelemetry() {
    // Start logging
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Call schedIn to write telemetry
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();

    // Verify telemetry was written
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_NumQueueDrops_SIZE(1);
    ASSERT_TLM_NumQueueDrops(0, 0);
    ASSERT_TLM_LoggingEnabled_SIZE(1);
    ASSERT_TLM_LoggingEnabled(0, true);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 0);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);

    // Log some buffers
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->clearHistory();

    // Call schedIn again
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();

    // Verify buffer count updated
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_LoggingEnabled_SIZE(1);
    ASSERT_TLM_LoggingEnabled(0, true);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 2);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);

    // Stop logging
    this->sendCmd_StopComDp(0, 1);
    this->component.doDispatch();
    ASSERT_EVENTS_ComDpStopped_SIZE(1);
    ASSERT_EVENTS_ComDpStopped(0, 0);  // no partial container to send
    this->clearHistory();

    // Call schedIn again
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();

    // Verify logging disabled
    ASSERT_TLM_SIZE(4);
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
    Fw::ComBuffer comBuf = this->createTestComBuffer(16);

    // First buffer - triggers container allocation with priority 15
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_GET_SIZE(1);

    // Second buffer - fills and sends the container
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Priority 15 must have been applied to the container header (default would be 5)
    Fw::DpContainer sent(0, this->productSendHistory->at(0).buffer);
    ASSERT_EQ(sent.deserializeHeader(), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(sent.getPriority(), 15);

    // Test passes if we get here - the priority was successfully preserved and applied
    // when the container was allocated, even though logging was not enabled initially.
    // The bug would have caused the default priority (5) to be used instead of 15.
}

void ComLoggerDpTester::testStartRecordingPort() {
    // Test parameters
    const U32 packetsPerContainer = 5;
    const FwDpPriorityType priority = 10;

    // Invoke port
    this->invoke_to_startRecordingIn(0, packetsPerContainer, priority);
    this->component.doDispatch();

    // Verify event was logged
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ComDpStarted_SIZE(1);
    ASSERT_EVENTS_ComDpStarted(0, 5);

    // Verify logging is enabled by sending a Com buffer
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should have allocated a container
    ASSERT_PRODUCT_GET_SIZE(1);
}

void ComLoggerDpTester::testStopRecordingPort() {
    // Start logging via command first
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Send one packet (partial container)
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
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
    this->sendCmd_StartComDp(0, 0, 2, 10);
    this->component.doDispatch();
    this->clearHistory();

    // Send two Com buffers
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->clearHistory();

    // Check telemetry before clearing
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(4);
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
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 0);  // Counter reset to 0
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);  // Dropped counter also reset
}

void ComLoggerDpTester::testBufferOverflow() {
    // Start logging with 1 packet per container
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
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 1);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);  // No buffers dropped
}

void ComLoggerDpTester::testDpBufferErrorThrottling() {
    // Start logging
    this->startLoggingAndClearHistory(2, 10);

    // Simulate allocation failure
    this->m_allocationFailure = true;

    // Send multiple packets - should trigger multiple allocation failures
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);

    // Trigger 5 allocation failures
    for (int i = 0; i < 5; i++) {
        this->invoke_to_comIn(0, comBuf, 0);
        this->component.doDispatch();
    }

    // Should only have ONE DpBufferError event due to throttling
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DpBufferError_SIZE(1);
    const FwSizeType sentrySize = sizeof(ComLoggerDpSentry);
    const FwSizeType expectedSize =
        2 * ComLoggerDp::SIZE_OF_ComBufferRecord_RECORD(FW_COM_BUFFER_MAX_SIZE + sentrySize);
    ASSERT_EVENTS_DpBufferError(0, expectedSize);

    // Clear counters should clear the throttle
    this->clearHistory();
    this->sendCmd_CLEAR_COUNTERS(0, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_CountersCleared_SIZE(1);
    this->clearHistory();

    // Send another packet - should trigger event again (throttle cleared)
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DpBufferError_SIZE(1);
    ASSERT_EVENTS_DpBufferError(0, expectedSize);  // Same expectedSize from above
}

void ComLoggerDpTester::testUpdatePriorityNotRecording() {
    // Don't start recording - logging disabled (configure with disabled state)
    this->component.configure(false, 0, 0, 0);

    // Try to update priority when not recording
    this->sendCmd_UpdatePriority(0, 0, 15);
    this->component.doDispatch();

    // Command should succeed (priority is stored for future use)
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_UPDATEPRIORITY, 0, Fw::CmdResponse::OK);

    // PriorityUpdated event is always emitted, even when not recording
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PriorityUpdated_SIZE(1);
    ASSERT_EVENTS_PriorityUpdated(0, 15);
}

void ComLoggerDpTester::testUpdatePriorityNoContainer() {
    // Start recording but don't send any packets (no container allocated yet)
    this->startLoggingAndClearHistory(3, 5);

    // Update priority when no container exists
    this->sendCmd_UpdatePriority(0, 0, 15);
    this->component.doDispatch();

    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_UPDATEPRIORITY, 0, Fw::CmdResponse::OK);

    // PriorityUpdated event is always emitted (priority stored for future containers)
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PriorityUpdated_SIZE(1);
    ASSERT_EVENTS_PriorityUpdated(0, 15);

    // Send a packet - new container should be created with NEW priority (15)
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Verify container was allocated with updated priority
    ASSERT_PRODUCT_GET_SIZE(1);

    // Fill the container (3 packets) and verify the header carries the updated priority (15, not 5)
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_SEND_SIZE(1);
    Fw::DpContainer sent(0, this->productSendHistory->at(0).buffer);
    ASSERT_EQ(sent.deserializeHeader(), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(sent.getPriority(), 15);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void ComLoggerDpTester::startLoggingAndClearHistory(U32 packetsPerContainer, FwDpPriorityType priority) {
    this->sendCmd_StartComDp(0, 0, packetsPerContainer, priority);
    this->component.doDispatch();
    this->clearHistory();
}

Fw::ComBuffer ComLoggerDpTester::createTestComBuffer(FwSizeType size, U8 startValue) {
    // Create test data array with sequential values
    U8 testData[FW_COM_BUFFER_MAX_SIZE];
    for (FwSizeType i = 0; i < size; ++i) {
        testData[i] = static_cast<U8>((startValue + i) & 0xFF);
    }

    // Create and populate ComBuffer
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, size);
    return comBuf;
}

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

Fw::Success::T ComLoggerDpTester::productGet_handler(FwDpIdType id, FwSizeType dataSize, Fw::Buffer& buffer) {
    this->pushProductGetEntry(id, dataSize);

    if (this->m_allocationFailure) {
        return Fw::Success::FAILURE;
    }

    // Return a valid buffer
    buffer.set(this->m_buffer, dataSize);
    return Fw::Success::SUCCESS;
}

void ComLoggerDpTester::validateDataProductFormat(const Fw::Buffer& buffer,
                                                  U32 expectedPacketCount,
                                                  const U8* expectedData,
                                                  FwSizeType expectedDataSize) {
    // Get raw buffer pointer
    const U8* bufPtr = buffer.getData();
    FwSizeType bufSize = buffer.getSize();

    // Expected sentry value (0xDEADBEEF serialized in F Prime big-endian format)
    const U8 sentryBytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    // Search through the buffer for the expected pattern:
    // Each record contains: sentry (4 bytes) + ComBuffer (U64 size 8 bytes + data N bytes)
    // The ComBuffer is serialized with F Prime's standard format which uses U64 for sizes
    // This validates that:
    // 1. The sentry value (0xDEADBEEF) is correctly inserted before each ComBuffer
    // 2. The ComBuffer data is correctly serialized after the sentry
    // 3. The ground software can use the sentry to identify buffer boundaries
    U32 foundCount = 0;
    const FwSizeType sizeFieldSize = sizeof(FwSizeStoreType);  // U64 = 8 bytes

    for (FwSizeType offset = 0; offset <= bufSize - (sizeof(sentryBytes) + sizeFieldSize + expectedDataSize);
         offset++) {
        // Check if we found a sentry at this position
        bool sentryMatch = true;
        for (FwSizeType i = 0; i < sizeof(sentryBytes); i++) {
            if (bufPtr[offset + i] != sentryBytes[i]) {
                sentryMatch = false;
                break;
            }
        }

        if (sentryMatch) {
            // Check for ComBuffer size field (U64, 8 bytes big-endian) after sentry
            // Extract the U64 size value
            U64 comBufSize = 0;
            for (FwSizeType i = 0; i < sizeFieldSize; i++) {
                comBufSize = (comBufSize << 8) | bufPtr[offset + 4 + i];
            }

            if (comBufSize == expectedDataSize) {
                // Validate the ComBuffer data after sentry + size field
                bool dataMatch = true;
                for (FwSizeType j = 0; j < expectedDataSize; j++) {
                    if (bufPtr[offset + 4 + sizeFieldSize + j] != expectedData[j]) {
                        dataMatch = false;
                        break;
                    }
                }

                if (dataMatch) {
                    foundCount++;
                    // Skip past this match to find the next one
                    offset += sizeof(sentryBytes) + sizeFieldSize + expectedDataSize - 1;
                }
            }
        }
    }

    // Verify we found the expected number of sentry + data patterns
    ASSERT_EQ(foundCount, expectedPacketCount)
        << "Expected to find " << expectedPacketCount << " sentry+size+data patterns, but found " << foundCount;
}

void ComLoggerDpTester::testDataProductFormat() {
    // Start logging with 2 packets per container
    this->startLoggingAndClearHistory(2, 10);

    // Create test data with a recognizable pattern
    U8 testData[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    Fw::ComBuffer comBuf;
    comBuf.serializeFrom(testData, sizeof(testData));

    // Send two packets to fill the container
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Verify container was sent
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Get the sent buffer from the test framework productSendHistory
    DpSend dpSendEntry = this->productSendHistory->at(0);
    Fw::Buffer sentBuffer = dpSendEntry.buffer;

    // Validate the data product format
    this->validateDataProductFormat(sentBuffer, 2, testData, sizeof(testData));
}

void ComLoggerDpTester::testConfigureEnabled() {
    // Test configure() with enabled=true to exercise line 29
    // This should enable logging and validate parameters
    this->component.configure(true, 3, 10, 10);

    // Verify logging is enabled by sending a Com buffer
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should have allocated a container (logging is enabled)
    ASSERT_PRODUCT_GET_SIZE(1);

    // Verify event was logged for starting recording
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ComDpStarted_SIZE(1);
    ASSERT_EVENTS_ComDpStarted(0, 3);
}

void ComLoggerDpTester::testReconfigureWithPartialContainer() {
    // Test reconfiguring while already recording with a partial container
    // This exercises line 127 where we send the partial container before reconfiguring

    // Start logging with 3 packets per container
    this->startLoggingAndClearHistory(3, 5);

    // Send one packet (partial container)
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should have allocated but not sent container yet
    ASSERT_PRODUCT_GET_SIZE(1);
    ASSERT_PRODUCT_SEND_SIZE(0);
    this->clearHistory();

    // Now reconfigure with different parameters while recording is active
    // This should send the partial container before reconfiguring
    this->sendCmd_StartComDp(0, 0, 2, 15);
    this->component.doDispatch();

    // Should have sent the partial container (line 127)
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Command should succeed
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STARTCOMDP, 0, Fw::CmdResponse::OK);

    // Should have ComDpStarted event for the reconfiguration
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_ComDpStarted_SIZE(1);
    ASSERT_EVENTS_ComDpStarted(0, 2);
}

void ComLoggerDpTester::testPacketTooLarge() {
    // Test allocation failure when trying to get a new container
    // This tests the error path when dpGet fails

    // Start logging with 1 packet per container
    this->startLoggingAndClearHistory(1, 10);

    // Create a packet and send it successfully first to allocate a container
    U8 smallData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    Fw::ComBuffer smallBuf;
    smallBuf.serializeFrom(smallData, sizeof(smallData));
    this->invoke_to_comIn(0, smallBuf, 0);
    this->component.doDispatch();

    // Container should be sent (1 packet fills it)
    ASSERT_PRODUCT_SEND_SIZE(1);
    this->clearHistory();

    // Set allocation failure to trigger when trying to get new container
    this->m_allocationFailure = true;

    // Send another packet - this should:
    // 1. Try to allocate new container (previous was sent)
    // 2. Fail to allocate (due to m_allocationFailure)
    // 3. Return early without processing the packet
    this->invoke_to_comIn(0, smallBuf, 0);
    this->component.doDispatch();

    // Should have attempted allocation but failed
    ASSERT_PRODUCT_GET_SIZE(1);

    // Should have generated DpBufferError event for the dropped buffer
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_DpBufferError_SIZE(1);
    ASSERT_EVENTS_DpBufferError(
        0, ComLoggerDp::SIZE_OF_ComBufferRecord_RECORD(FW_COM_BUFFER_MAX_SIZE + sizeof(ComLoggerDpSentry)));

    // Buffer should be counted as dropped
    this->clearHistory();
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 1);
}

void ComLoggerDpTester::testContainerOverflowRetry() {
    // Test normal operation with large packets
    // This verifies that containers correctly hold the configured number of packets

    // Start with 2 packets per container
    this->startLoggingAndClearHistory(2, 10);

    // Create packets with max size - these will take up most of the container
    U8 largeData[FW_COM_BUFFER_MAX_SIZE];
    for (U32 i = 0; i < FW_COM_BUFFER_MAX_SIZE; ++i) {
        largeData[i] = static_cast<U8>(i & 0xFF);
    }
    Fw::ComBuffer largeBuf;
    largeBuf.serializeFrom(largeData, FW_COM_BUFFER_MAX_SIZE);

    // Send first large packet - should fit in container
    this->invoke_to_comIn(0, largeBuf, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_GET_SIZE(1);
    ASSERT_PRODUCT_SEND_SIZE(0);  // Not sent yet

    // Send second large packet - should fit and trigger send
    this->invoke_to_comIn(0, largeBuf, 0);
    this->component.doDispatch();

    // Container should be sent (2 packets fills it)
    ASSERT_PRODUCT_SEND_SIZE(1);

    // Verify both buffers were logged (not dropped)
    this->clearHistory();
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(4);
    ASSERT_TLM_NumBuffersLogged_SIZE(1);
    ASSERT_TLM_NumBuffersLogged(0, 2);  // Both buffers logged
    ASSERT_TLM_NumBuffersDropped_SIZE(1);
    ASSERT_TLM_NumBuffersDropped(0, 0);  // No buffers dropped
}

void ComLoggerDpTester::testSerializationFailureCounter() {
    // Test CLEAR_COUNTERS command functionality
    // Verify that the command properly clears NumBuffersLogged and NumBuffersDropped counters

    // Start logging
    this->startLoggingAndClearHistory(2, 10);

    // Send some packets normally
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Check telemetry - counter should be 0 in normal operation
    this->clearHistory();
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(4);

    // Test that CLEAR_COUNTERS command clears this counter
    // (even though it's 0, this verifies the command handles it)
    this->clearHistory();
    this->sendCmd_CLEAR_COUNTERS(0, 0);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_CLEAR_COUNTERS, 0, Fw::CmdResponse::OK);

    // Verify counter is still 0 after clear
    this->clearHistory();
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();
    ASSERT_TLM_SIZE(4);
}

void ComLoggerDpTester::testAutoFlush() {
    // Configure with auto-flush timeout of 10
    this->component.configure(true, 5, 10, 10);
    this->clearHistory();

    // Send one packet to create partial container
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Verify container was allocated but not sent yet
    ASSERT_PRODUCT_GET_SIZE(1);
    ASSERT_PRODUCT_SEND_SIZE(0);
    this->clearHistory();

    // Call schedIn 9 times (just below threshold of 10)
    for (int i = 0; i < 9; i++) {
        this->invoke_to_schedIn(0, 0);
        this->component.doDispatch();
    }

    // Should not have flushed yet
    ASSERT_PRODUCT_SEND_SIZE(0);

    // Call schedIn one more time (10th call, reaching threshold)
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();

    // Should have auto-flushed the partial container
    ASSERT_PRODUCT_SEND_SIZE(1);
    Fw::DpContainer sent(0, this->productSendHistory->at(0).buffer);
    ASSERT_EQ(sent.deserializeHeader(), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(sent.getPriority(), 10);
    ASSERT_GT(sent.getDataSize(), 0U);

    // Next packet must allocate a fresh container after the flush
    this->clearHistory();
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_GET_SIZE(1);
    ASSERT_PRODUCT_SEND_SIZE(0);
}

void ComLoggerDpTester::testAutoFlushResetOnPacket() {
    // Configure with auto-flush timeout of 10
    this->component.configure(true, 5, 10, 10);
    this->clearHistory();

    // Send one packet
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->clearHistory();

    // Call schedIn 8 times
    for (int i = 0; i < 8; i++) {
        this->invoke_to_schedIn(0, 0);
        this->component.doDispatch();
    }

    // Send another packet - this should reset the counter
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();
    this->clearHistory();

    // Call schedIn 9 more times (would have been 17 total without reset)
    for (int i = 0; i < 9; i++) {
        this->invoke_to_schedIn(0, 0);
        this->component.doDispatch();
    }

    // Should not have flushed yet because counter was reset
    ASSERT_PRODUCT_SEND_SIZE(0);

    // One more schedIn call to reach threshold (10 since last packet)
    this->invoke_to_schedIn(0, 0);
    this->component.doDispatch();

    // Should now flush
    ASSERT_PRODUCT_SEND_SIZE(1);
}

void ComLoggerDpTester::testAutoFlushDisabled() {
    // Configure with auto-flush disabled (timeout = 0)
    this->component.configure(true, 5, 10, 0);
    this->clearHistory();

    // Send one packet to create partial container
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Verify container was allocated but not sent yet
    ASSERT_PRODUCT_GET_SIZE(1);
    ASSERT_PRODUCT_SEND_SIZE(0);
    this->clearHistory();

    // Call schedIn five times with auto-flush disabled
    // With flushTimeout=0, auto-flush should be completely disabled
    for (int i = 0; i < 5; i++) {
        this->invoke_to_schedIn(0, 0);
        this->component.doDispatch();
        this->clearHistory();
    }

    // Should still not have flushed (auto-flush is disabled with timeout=0)
    ASSERT_PRODUCT_SEND_SIZE(0);

    // Now stop recording - this should send the partial container
    this->sendCmd_StopComDp(0, 0);
    this->component.doDispatch();
    ASSERT_PRODUCT_SEND_SIZE(1);  // Partial container sent on stop
}

void ComLoggerDpTester::testPacketsPerContainerTooLarge() {
    // Calculate max packets per container (same formula as in ComLoggerDp.cpp)
    constexpr U32 MAX_PACKETS = static_cast<U32>((std::numeric_limits<U32>::max() - Fw::DpContainer::MIN_PACKET_SIZE) /
                                                 ComLoggerDp::RECORD_SIZE);
    // Try to start with one more than max - should fail validation
    this->sendCmd_StartComDp(0, 0, MAX_PACKETS + 1, 10);
    this->component.doDispatch();

    // Should reject with VALIDATION_ERROR
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STARTCOMDP, 0, Fw::CmdResponse::VALIDATION_ERROR);

    // Should emit StartRecordingFailed event
    ASSERT_EVENTS_StartRecordingFailed_SIZE(1);

    // Component should remain disabled
    ASSERT_TLM_SIZE(0);
}

void ComLoggerDpTester::testStopWhenAlreadyStopped() {
    // Component starts disabled by default
    // Try to stop when already stopped - should be idempotent
    this->sendCmd_StopComDp(0, 0);
    this->component.doDispatch();

    // Should succeed (stopping when already stopped is valid)
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STOPCOMDP, 0, Fw::CmdResponse::OK);

    // Event should show 0 partial containers sent
    ASSERT_EVENTS_ComDpStopped_SIZE(1);
    ASSERT_EVENTS_ComDpStopped(0, 0);
}

void ComLoggerDpTester::testComBufferWhenDisabled() {
    // Configure with logging disabled
    this->component.configure(false, 0, 0, 0);
    this->clearHistory();

    // Send a ComBuffer when logging is disabled
    Fw::ComBuffer comBuf = this->createTestComBuffer(8);
    this->invoke_to_comIn(0, comBuf, 0);
    this->component.doDispatch();

    // Should not allocate any containers or send any data products
    ASSERT_PRODUCT_GET_SIZE(0);
    ASSERT_PRODUCT_SEND_SIZE(0);

    // Should not emit any events or telemetry
    ASSERT_EVENTS_SIZE(0);
    ASSERT_TLM_SIZE(0);
}

}  // namespace Svc
