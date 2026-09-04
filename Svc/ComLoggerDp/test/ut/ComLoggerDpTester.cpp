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

    // Test validation error with zero packets per container
    this->clearHistory();
    this->sendCmd_StartComDp(0, 1, 0, 10);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, ComLoggerDp::OPCODE_STARTCOMDP, 1, Fw::CmdResponse::VALIDATION_ERROR);
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

    // Send a packet
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
