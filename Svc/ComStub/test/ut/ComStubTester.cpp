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

ComStubTester ::ComStubTester(TestMode test_mode)
    : ComStubGTestBase("Tester", MAX_HISTORY_SIZE), component("ComStub"), m_test_mode(test_mode), m_retries(0) {
    this->initComponents();
    if (test_mode == TestMode::UNSPECIFIED) {
        // Unspecified test mode -> connects all ports
        this->connectPorts();
    } else {
        // Connect sync or async ports based on sync/async test mode
        this->connectPortsWithTestMode(test_mode);
    }
}

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
    if (this->m_test_mode == TestMode::SYNC) {
        this->m_sync_send_status = Drv::ByteStreamStatus::OP_OK;
        invoke_to_dataIn(0, buffer, context);
        ASSERT_from_drvSendOut_SIZE(1);
        ASSERT_from_drvSendOut(0, buffer);
    } else if (this->m_test_mode == TestMode::ASYNC) {
        invoke_to_dataIn(0, buffer, context);
        ASSERT_from_drvAsyncSendOut_SIZE(1);
        ASSERT_from_drvAsyncSendOut(0, buffer);
    }

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
    ComCfg::FrameContext context;

    // Downlink
    if (this->m_test_mode == TestMode::SYNC) {
        this->m_sync_send_status = Drv::ByteStreamStatus::OTHER_ERROR;
        invoke_to_dataIn(0, buffer, context);
        ASSERT_from_drvSendOut_SIZE(1);     // no retry on error (only one send)
        ASSERT_from_dataReturnOut_SIZE(1);  // no drvSendOut sent when failure
    } else if (this->m_test_mode == TestMode::ASYNC) {
        invoke_to_dataIn(0, buffer, context);
        ASSERT_from_drvAsyncSendOut_SIZE(1);
    }

    // Uplink
    invoke_to_drvReceiveIn(0, buffer, Drv::ByteStreamStatus::OTHER_ERROR);
    ASSERT_from_dataOut_SIZE(0);              // receiving failure should not send anything
    ASSERT_from_drvReceiveReturnOut_SIZE(1);  // return the buffer ownership
}

void ComStubTester ::test_retry_async() {
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
        invoke_to_drvAsyncSendReturnIn(0, buffers[i], Drv::ByteStreamStatus::SEND_RETRY);
        // Test we have indeed retried (data sent on drvAsyncSendOut)
        ASSERT_from_drvAsyncSendOut_SIZE(static_cast<U32>(i + 1));
        ASSERT_from_drvAsyncSendOut(static_cast<U32>(i), buffers[i]);
    }
    ASSERT_from_drvAsyncSendOut_SIZE(static_cast<U32>(this->component.RETRY_LIMIT));
    ASSERT_EQ(this->component.m_retry_count, this->component.RETRY_LIMIT);
    // Retry one more time should block from retrying and reset retry count
    invoke_to_drvAsyncSendReturnIn(0, buffers[MAX_ITERS - 1], Drv::ByteStreamStatus::SEND_RETRY);
    ASSERT_from_drvAsyncSendOut_SIZE(
        static_cast<U32>(this->component.RETRY_LIMIT));  // no drvAsyncSendOut sent when SEND_RETRY
    ASSERT_from_dataReturnOut_SIZE(1);                   // buffer ownership was returned
    ASSERT_EQ(this->component.m_retry_count, 0);
}

void ComStubTester ::test_retry_sync() {
    this->test_initial();

    U8 storage[8];
    Fw::Buffer buffer(storage, sizeof(storage));
    this->fill(buffer);
    ComCfg::FrameContext context;

    this->m_sync_send_status = Drv::ByteStreamStatus::SEND_RETRY;
    this->m_retry_fail = true;
    invoke_to_dataIn(0, buffer, context);
    ASSERT_from_drvSendOut_SIZE(static_cast<U32>(this->component.RETRY_LIMIT));
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer, context);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success::FAILURE);
}

void ComStubTester ::test_retry_reset_sync() {
    this->test_initial();

    U8 storage[8];
    Fw::Buffer buffer(storage, sizeof(storage));
    this->fill(buffer);
    ComCfg::FrameContext context;

    this->m_sync_send_status = Drv::ByteStreamStatus::SEND_RETRY;
    this->m_retry_fail = false;  // last retry will succeed with OP_OK
    invoke_to_dataIn(0, buffer, context);
    ASSERT_from_drvSendOut_SIZE(static_cast<U32>(this->component.RETRY_LIMIT));
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer, context);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success::SUCCESS);
    // Now that we succeeded, we can send again and expect success
    this->m_sync_send_status = Drv::ByteStreamStatus::OP_OK;
    invoke_to_dataIn(0, buffer, context);
    ASSERT_from_drvSendOut_SIZE(static_cast<U32>(this->component.RETRY_LIMIT) + 1);
    ASSERT_from_dataReturnOut_SIZE(2);
    ASSERT_from_dataReturnOut(1, buffer, context);
    ASSERT_from_comStatusOut_SIZE(2);
    ASSERT_from_comStatusOut(1, Fw::Success::SUCCESS);
}

void ComStubTester ::test_retry_reset_async() {
    this->test_initial();
    FwIndexType MAX_ITERS = static_cast<FwIndexType>(this->component.RETRY_LIMIT + 1);
    U32 expected_drvAsyncSendOut_count = 0;

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
        invoke_to_drvAsyncSendReturnIn(0, buffers[i], Drv::ByteStreamStatus::SEND_RETRY);
        ASSERT_from_drvAsyncSendOut(expected_drvAsyncSendOut_count, buffers[i]);
        expected_drvAsyncSendOut_count++;  // trick: increment now to use as index prior and size after
        ASSERT_from_drvAsyncSendOut_SIZE(expected_drvAsyncSendOut_count);
    }
    // Now, we receive a OP_OK, which should not retry (drvAsyncSendOut should not be called) and reset the retry count
    ASSERT_from_drvAsyncSendOut_SIZE(expected_drvAsyncSendOut_count);  // no drvAsyncSendOut sent when OP_OK
    invoke_to_drvAsyncSendReturnIn(0, buffers[0], Drv::ByteStreamStatus::OP_OK);
    ASSERT_from_drvAsyncSendOut_SIZE(expected_drvAsyncSendOut_count);  // no drvAsyncSendOut sent when OP_OK
    // Now that retry count is reset, we can retry again without a problem
    for (FwIndexType i = 0; i < this->component.RETRY_LIMIT; i++) {
        invoke_to_drvAsyncSendReturnIn(0, buffers[i], Drv::ByteStreamStatus::SEND_RETRY);
        ASSERT_from_drvAsyncSendOut(expected_drvAsyncSendOut_count, buffers[i]);
        expected_drvAsyncSendOut_count++;  // trick: increment now to use as index prior and size after
        ASSERT_from_drvAsyncSendOut_SIZE(expected_drvAsyncSendOut_count);
    }
    ASSERT_from_drvAsyncSendOut_SIZE(expected_drvAsyncSendOut_count);  // no drvAsyncSendOut sent when OP_OK
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
// Helper for connecting ports based on test mode (sync vs async)
// ----------------------------------------------------------------------

void ComStubTester ::connectPortsWithTestMode(TestMode mode) {
    // Connect common typed input ports
    this->connect_to_dataIn(0, this->component.get_dataIn_InputPort(0));
    this->connect_to_dataReturnIn(0, this->component.get_dataReturnIn_InputPort(0));
    this->connect_to_drvConnected(0, this->component.get_drvConnected_InputPort(0));
    this->connect_to_drvReceiveIn(0, this->component.get_drvReceiveIn_InputPort(0));
    // Connect common typed output ports
    this->component.set_comStatusOut_OutputPort(0, this->get_from_comStatusOut(0));
    this->component.set_dataOut_OutputPort(0, this->get_from_dataOut(0));
    this->component.set_dataReturnOut_OutputPort(0, this->get_from_dataReturnOut(0));
    this->component.set_drvReceiveReturnOut_OutputPort(0, this->get_from_drvReceiveReturnOut(0));
    if (mode == TestMode::SYNC) {
        // Connect synchronous send port
        this->component.set_drvSendOut_OutputPort(0, this->get_from_drvSendOut(0));
    } else if (mode == TestMode::ASYNC) {
        // Connect Asynchronous send ports
        this->component.set_drvAsyncSendOut_OutputPort(0, this->get_from_drvAsyncSendOut(0));
        this->connect_to_drvAsyncSendReturnIn(0, this->component.get_drvAsyncSendReturnIn_InputPort(0));
    }
}

// ----------------------------------------------------------------------
// Helpers for output port test harness
// ----------------------------------------------------------------------

Drv::ByteStreamStatus ComStubTester ::from_drvSendOut_handler(const FwIndexType portNum, Fw::Buffer& sendBuffer) {
    this->pushFromPortEntry_drvSendOut(sendBuffer);
    // return m_sync_send_status by default, unless number of retries is exceeded
    if (this->m_sync_send_status == Drv::ByteStreamStatus::SEND_RETRY) {
        this->m_retries++;
    }
    if (this->m_retries >= this->component.RETRY_LIMIT and !this->m_retry_fail) {
        return Drv::ByteStreamStatus::OP_OK;  // if limit exceeded and no retry fail, return success
    }
    return this->m_sync_send_status;
}

}  // end namespace Svc
