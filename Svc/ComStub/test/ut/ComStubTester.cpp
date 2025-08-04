// ======================================================================
// \title  ComStub.hpp
// \author mstarch
// \brief  cpp file for ComStub test harness implementation class
// ======================================================================

#include "ComStubTester.hpp"
#include <STest/Pick/Pick.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ComStubTester ::ComStubTester()
    : ComStubGTestBase("Tester", MAX_HISTORY_SIZE),
      component("ComStub"),
      m_send_mode(Drv::ByteStreamStatus::OP_OK),
      m_retries(0) {
    this->initComponents();
    this->connectPorts();
}

ComStubTester ::~ComStubTester() {}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------
void ComStubTester ::fill(Fw::Buffer& buffer_to_fill) {
    U8 size = static_cast<U8>(STest::Pick::lowerUpper(1, sizeof(buffer_to_fill.getSize())));
    for (U32 i = 0; i < size; i++) {
        buffer_to_fill.getData()[i] = static_cast<U8>(STest::Pick::any());
    }
    buffer_to_fill.setSize(size);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------
void ComStubTester ::test_initial() {
    Fw::Success condition = Fw::Success::SUCCESS;
    invoke_to_drvConnected(0);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, condition);
    this->fromPortHistory_comStatusOut->clear();
}

void ComStubTester ::test_basic() {
    this->test_initial();
    U8 storage[8];
    Fw::Buffer buffer(storage, sizeof(storage));
    Fw::Success condition = Fw::Success::SUCCESS;
    ComCfg::FrameContext context;
    this->fill(buffer);

    // Downlink
    invoke_to_dataIn(0, buffer, context);
    ASSERT_from_drvSendOut_SIZE(1);
    ASSERT_from_drvSendOut(0, buffer);

    // Uplink
    ComCfg::FrameContext emptyContext;
    invoke_to_drvReceiveIn(0, buffer, Drv::ByteStreamStatus::OP_OK);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataOut(0, buffer, emptyContext);
}

void ComStubTester ::test_fail() {
    this->test_initial();
    U8 storage[8];
    Fw::Buffer buffer(storage, sizeof(storage));
    this->fill(buffer);
    Fw::Success condition = Fw::Success::FAILURE;
    m_send_mode = Drv::ByteStreamStatus::OTHER_ERROR;
    ComCfg::FrameContext context;

    // Downlink
    invoke_to_dataIn(0, buffer, context);
    ASSERT_from_drvSendOut_SIZE(1);
    ASSERT_from_drvSendOut(0, buffer);

    // Uplink
    invoke_to_drvReceiveIn(0, buffer, Drv::ByteStreamStatus::OTHER_ERROR);
    ASSERT_from_dataOut_SIZE(0);  // receiving failure should not send anything
}

void ComStubTester ::test_retry() {
    this->test_initial();
    FwIndexType MAX_ITERS = static_cast<FwIndexType>(this->component.RETRY_LIMIT + 1);

    // Make small individual buffers for testing
    U8 storage[MAX_ITERS][8];
    Fw::Buffer buffers[MAX_ITERS];
    for (FwIndexType i = 0; i < MAX_ITERS; i++) {
        buffers[i].setData(storage[i]);
        buffers[i].setSize(sizeof(storage[i]));
        buffers[i].setContext(static_cast<U32>(i));
        this->fill(buffers[i]);
    }
    // Retrying for as many times as the RETRY_LIMIT should be ok
    for (FwIndexType i = 0; i < this->component.RETRY_LIMIT; i++) {
        invoke_to_drvSendReturnIn(0, buffers[i], Drv::ByteStreamStatus::SEND_RETRY);
        // Test we have indeed retried (data sent on drvSendOut)
        ASSERT_from_drvSendOut_SIZE(static_cast<U32>(i + 1));
        ASSERT_from_drvSendOut(static_cast<U32>(i), buffers[i]);
    }
    ASSERT_from_drvSendOut_SIZE(static_cast<U32>(this->component.RETRY_LIMIT));
    ASSERT_EQ(this->component.m_retry_count, this->component.RETRY_LIMIT);
    // Retry one more time should block from retrying and reset retry count
    invoke_to_drvSendReturnIn(0, buffers[MAX_ITERS - 1], Drv::ByteStreamStatus::SEND_RETRY);
    ASSERT_from_drvSendOut_SIZE(static_cast<U32>(this->component.RETRY_LIMIT));  // no drvSendOut sent when SEND_RETRY
    ASSERT_from_dataReturnOut_SIZE(1);                                           // buffer ownership was returned
    ASSERT_EQ(this->component.m_retry_count, 0);
}

void ComStubTester ::test_retry_reset() {
    this->test_initial();
    FwIndexType MAX_ITERS = static_cast<FwIndexType>(this->component.RETRY_LIMIT + 1);
    U32 expected_drvSendOut_count = 0;

    // Make small individual buffers for testing
    U8 storage[MAX_ITERS][8];
    Fw::Buffer buffers[MAX_ITERS];
    for (FwIndexType i = 0; i < MAX_ITERS; i++) {
        buffers[i].setData(storage[i]);
        buffers[i].setSize(sizeof(storage[i]));
        buffers[i].setContext(static_cast<U32>(i));
        this->fill(buffers[i]);
    }

    // Retrying for as many times as the RETRY_LIMIT should be ok
    for (FwIndexType i = 0; i < this->component.RETRY_LIMIT; i++) {
        invoke_to_drvSendReturnIn(0, buffers[i], Drv::ByteStreamStatus::SEND_RETRY);
        ASSERT_from_drvSendOut(expected_drvSendOut_count, buffers[i]);
        expected_drvSendOut_count++;  // trick: increment now to use as index prior and size after
        ASSERT_from_drvSendOut_SIZE(expected_drvSendOut_count);
    }
    // Now, we receive a OP_OK, which should not retry (drvSendOut should not be called) and reset the retry count
    ASSERT_from_drvSendOut_SIZE(expected_drvSendOut_count);  // no drvSendOut sent when OP_OK
    invoke_to_drvSendReturnIn(0, buffers[0], Drv::ByteStreamStatus::OP_OK);
    ASSERT_from_drvSendOut_SIZE(expected_drvSendOut_count);  // no drvSendOut sent when OP_OK
    // Now that retry count is reset, we can retry again without a problem
    for (FwIndexType i = 0; i < this->component.RETRY_LIMIT; i++) {
        invoke_to_drvSendReturnIn(0, buffers[i], Drv::ByteStreamStatus::SEND_RETRY);
        ASSERT_from_drvSendOut(expected_drvSendOut_count, buffers[i]);
        expected_drvSendOut_count++;  // trick: increment now to use as index prior and size after
        ASSERT_from_drvSendOut_SIZE(expected_drvSendOut_count);
    }
    ASSERT_from_drvSendOut_SIZE(expected_drvSendOut_count);  // no drvSendOut sent when OP_OK
}

void ComStubTester ::test_buffer_return() {
    U8 data[1];
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext context;
    this->invoke_to_dataReturnIn(0, buffer, context);
    ASSERT_from_drvReceiveReturnOut_SIZE(1);  // incoming buffer should be returned
    ASSERT_EQ(this->fromPortHistory_drvReceiveReturnOut->at(0).fwBuffer.getData(), data);
    ASSERT_EQ(this->fromPortHistory_drvReceiveReturnOut->at(0).fwBuffer.getSize(), sizeof(data));
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void ComStubTester ::from_dataOut_handler(const FwIndexType portNum,
                                          Fw::Buffer& recvBuffer,
                                          const ComCfg::FrameContext& context) {
    this->pushFromPortEntry_dataOut(recvBuffer, context);
}

void ComStubTester ::from_comStatusOut_handler(const FwIndexType portNum, Fw::Success& condition) {
    this->pushFromPortEntry_comStatusOut(condition);
}

void ComStubTester ::from_drvSendOut_handler(const FwIndexType portNum, Fw::Buffer& sendBuffer) {
    this->pushFromPortEntry_drvSendOut(sendBuffer);
}

}  // end namespace Svc
