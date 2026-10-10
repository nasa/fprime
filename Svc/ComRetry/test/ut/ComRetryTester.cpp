// ======================================================================
// \title  ComRetryTester.cpp
// \author valdaarhun
// \brief  cpp file for ComRetry test harness implementation class
// ======================================================================

#include "ComRetryTester.hpp"
#include "Fw/Test/UnitTestAssert.hpp"

namespace Svc {

// Definitions for constants that are ODR-used by gtest assertions
const FwSizeType ComRetryTester::MAX_HISTORY_SIZE;
const FwSizeType ComRetryTester::TEST_INSTANCE_QUEUE_DEPTH;

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ComRetryTester ::ComRetryTester()
    : ComRetryGTestBase("ComRetryTester", ComRetryTester::MAX_HISTORY_SIZE),
      component("ComRetry"),
      m_inlineCount(0),
      m_inlineIndex(0),
      m_outputCount(0) {
    this->initComponents();
    this->connectPorts();
}

ComRetryTester ::~ComRetryTester() {
    this->component.deinit();
}

void ComRetryTester ::configure(U32 num_retries = 1) {
    component.configure(num_retries);
}

void ComRetryTester ::receiveBuffer(Fw::Buffer& buffer, ComCfg::FrameContext& context) {
    invoke_to_dataIn(0, buffer, context);
    dispatchAll();
    invoke_to_dataReturnIn(0, buffer, context);
    dispatchAll();
}

void ComRetryTester ::sendStatus(Fw::Success status) {
    invoke_to_comStatusIn(0, status);
    dispatchAll();
}

void ComRetryTester ::dispatchAll() {
    // Bounded: each dispatch may enqueue at most two messages (dataReturnIn, comStatusIn) via the inline adapter
    const FwSizeType limit = TEST_INSTANCE_QUEUE_DEPTH + 2 * MAX_INLINE_STATUSES;
    for (FwSizeType i = 0; (i < limit) && (this->queuedMessages() > 0); i++) {
        ASSERT_EQ(this->component.doDispatch(), ComRetryComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    }
    ASSERT_EQ(this->queuedMessages(), 0);
}

FwSizeType ComRetryTester ::queuedMessages() {
    return static_cast<FwSizeType>(this->component.m_queue.getMessagesAvailable());
}

void ComRetryTester ::setInlineAdapter(const Fw::Success* statuses, FwSizeType count) {
    FW_ASSERT(count <= MAX_INLINE_STATUSES, static_cast<FwAssertArgType>(count));
    for (FwSizeType i = 0; i < count; i++) {
        this->m_inlineStatuses[i] = statuses[i];
    }
    this->m_inlineCount = count;
    this->m_inlineIndex = 0;
}

void ComRetryTester ::checkDataOut(FwIndexType expectedIndex, U8* expectedData, FwSizeType expectedDataSize) {
    Fw::Buffer emittedBuffer = this->fromPortHistory_dataOut->at(expectedIndex).data;
    ASSERT_EQ(expectedDataSize, emittedBuffer.getSize());
    for (FwSizeType i = 0; i < expectedDataSize; i++) {
        ASSERT_EQ(emittedBuffer.getData()[i], expectedData[i]);
    }
}

void ComRetryTester ::checkOutputOrder(const OutputKind* expected, FwSizeType count) {
    ASSERT_EQ(this->m_outputCount, count);
    for (FwSizeType i = 0; i < count; i++) {
        ASSERT_EQ(this->m_outputOrder[i], expected[i]) << "Output " << i << " out of order";
    }
}

void ComRetryTester ::recordOutput(OutputKind kind) {
    ASSERT_LT(this->m_outputCount, MAX_HISTORY_SIZE);
    this->m_outputOrder[this->m_outputCount] = kind;
    this->m_outputCount++;
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void ComRetryTester ::from_dataOut_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    this->pushFromPortEntry_dataOut(data, context);
    this->recordOutput(DATA_OUT);
    // Emulate an adapter (e.g. a radio) answering from within the send call, on the ComRetry thread
    if (this->m_inlineIndex < this->m_inlineCount) {
        Fw::Success status = this->m_inlineStatuses[this->m_inlineIndex];
        this->m_inlineIndex++;
        this->invoke_to_dataReturnIn(0, data, context);
        this->invoke_to_comStatusIn(0, status);
    }
}

void ComRetryTester ::from_dataReturnOut_handler(FwIndexType portNum,
                                                 Fw::Buffer& data,
                                                 const ComCfg::FrameContext& context) {
    this->pushFromPortEntry_dataReturnOut(data, context);
    this->recordOutput(DATA_RETURN_OUT);
}

void ComRetryTester ::from_comStatusOut_handler(FwIndexType portNum, Fw::Success& condition) {
    this->pushFromPortEntry_comStatusOut(condition);
    this->recordOutput(COM_STATUS_OUT);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ComRetryTester ::testNullBuffer() {
    Fw::Success state = Fw::Success::SUCCESS;
    sendStatus(state);
    ASSERT_from_comStatusOut(0, state);

    state = Fw::Success::FAILURE;
    sendStatus(state);
    ASSERT_from_comStatusOut(1, state);
}

void ComRetryTester ::testBufferSend() {
    U8 data_a[BUFFER_LENGTH] = DATA_A;
    U8 data_b[BUFFER_LENGTH] = DATA_B;
    Fw::Buffer buffer_a(&data_a[0], sizeof(data_a));
    Fw::Buffer buffer_b(&data_b[0], sizeof(data_b));
    ComCfg::FrameContext nullContext;
    Fw::Success state = Fw::Success::SUCCESS;
    configure();

    receiveBuffer(buffer_a, nullContext);
    sendStatus(state);
    ASSERT_from_dataReturnOut(0, buffer_a, nullContext);
    ASSERT_from_comStatusOut(0, state);

    receiveBuffer(buffer_b, nullContext);
    sendStatus(state);
    ASSERT_from_dataReturnOut(1, buffer_b, nullContext);
    ASSERT_from_comStatusOut(1, state);

    checkDataOut(0, buffer_a.getData(), buffer_a.getSize());
    checkDataOut(1, buffer_b.getData(), buffer_b.getSize());
}

void ComRetryTester ::testBufferRetry() {
    U8 data_a[BUFFER_LENGTH] = DATA_A;
    U8 data_b[BUFFER_LENGTH] = DATA_B;
    Fw::Buffer buffer_a(&data_a[0], sizeof(data_a));
    Fw::Buffer buffer_b(&data_b[0], sizeof(data_b));
    ComCfg::FrameContext nullContext;
    Fw::Success state = Fw::Success::FAILURE;
    configure();

    receiveBuffer(buffer_a, nullContext);
    sendStatus(state);  // First delivery is a failure
    state = Fw::Success::SUCCESS;
    sendStatus(state);  // Downstream component is ready to receive buffer
    invoke_to_dataReturnIn(0, buffer_a, nullContext);
    dispatchAll();
    sendStatus(state);  // Redelivery is successful

    ASSERT_from_dataReturnOut(0, buffer_a, nullContext);
    ASSERT_from_comStatusOut(0, state);

    receiveBuffer(buffer_b, nullContext);
    sendStatus(state);
    ASSERT_from_dataReturnOut(1, buffer_b, nullContext);
    ASSERT_from_comStatusOut(1, state);

    checkDataOut(0, buffer_a.getData(), buffer_a.getSize());
    checkDataOut(1, buffer_a.getData(), buffer_a.getSize());
    checkDataOut(2, buffer_b.getData(), buffer_b.getSize());
}

void ComRetryTester ::testBufferRetryTillFailure() {
    U8 data_a[BUFFER_LENGTH] = DATA_A;
    U8 data_b[BUFFER_LENGTH] = DATA_B;
    Fw::Buffer buffer_a(&data_a[0], sizeof(data_a));
    Fw::Buffer buffer_b(&data_b[0], sizeof(data_b));
    ComCfg::FrameContext nullContext;
    Fw::Success failure = Fw::Success::FAILURE;
    Fw::Success success = Fw::Success::SUCCESS;

    FwIndexType num_retries = 3;  // This is also the default number of retries

    receiveBuffer(buffer_a, nullContext);
    sendStatus(failure);
    checkDataOut(0, buffer_a.getData(), buffer_a.getSize());

    for (FwIndexType i = 1; i <= num_retries; i++) {
        sendStatus(success);
        invoke_to_dataReturnIn(0, buffer_a, nullContext);
        dispatchAll();
        sendStatus(failure);
        checkDataOut(i, buffer_a.getData(), buffer_a.getSize());
    }

    ASSERT_from_dataReturnOut(0, buffer_a, nullContext);
    ASSERT_from_comStatusOut(0, failure);

    receiveBuffer(buffer_b, nullContext);
    sendStatus(success);
    ASSERT_from_dataReturnOut(1, buffer_b, nullContext);
    ASSERT_from_comStatusOut(1, success);
    checkDataOut(num_retries + 1, buffer_b.getData(), buffer_b.getSize());
}

void ComRetryTester ::testInputsQueued() {
    U8 data_a[BUFFER_LENGTH] = DATA_A;
    Fw::Buffer buffer_a(&data_a[0], sizeof(data_a));
    ComCfg::FrameContext nullContext;
    Fw::Success success = Fw::Success::SUCCESS;
    configure();

    // Invoking an input only enqueues: no work is done on the calling thread
    invoke_to_dataIn(0, buffer_a, nullContext);
    ASSERT_EQ(queuedMessages(), 1);
    ASSERT_from_dataOut_SIZE(0);
    dispatchAll();
    ASSERT_from_dataOut_SIZE(1);

    // Ownership return and status are queued and processed in arrival order
    invoke_to_dataReturnIn(0, buffer_a, nullContext);
    invoke_to_comStatusIn(0, success);
    ASSERT_EQ(queuedMessages(), 2);
    ASSERT_from_dataReturnOut_SIZE(0);
    ASSERT_from_comStatusOut_SIZE(0);
    dispatchAll();

    ASSERT_from_dataReturnOut(0, buffer_a, nullContext);
    ASSERT_from_comStatusOut(0, success);
    const OutputKind expected[] = {DATA_OUT, DATA_RETURN_OUT, COM_STATUS_OUT};
    checkOutputOrder(expected, FW_NUM_ARRAY_ELEMENTS(expected));
}

void ComRetryTester ::testSynchronousAdapterRetry() {
    ::Test::UnitTestAssert assertHook;
    U8 data_a[BUFFER_LENGTH] = DATA_A;
    Fw::Buffer buffer_a(&data_a[0], sizeof(data_a));
    ComCfg::FrameContext context;
    context.set_apid(ComCfg::Apid::FW_PACKET_FILE);
    const Fw::Success answers[] = {Fw::Success::FAILURE, Fw::Success::SUCCESS};
    setInlineAdapter(answers, FW_NUM_ARRAY_ELEMENTS(answers));
    configure();

    // Send: the adapter answers dataReturnIn + FAILURE from within dataOut, onto the ComRetry queue
    invoke_to_dataIn(0, buffer_a, context);
    ASSERT_EQ(this->component.doDispatch(), ComRetryComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(queuedMessages(), 2);

    // Worst-case backlog: a recovery SUCCESS from another thread and a health ping arrive before dispatch
    Fw::Success recovery = Fw::Success::SUCCESS;
    invoke_to_comStatusIn(0, recovery);
    invoke_to_pingIn(0, 0x1234);
    ASSERT_EQ(queuedMessages(), TEST_INSTANCE_QUEUE_DEPTH);
    ASSERT_FALSE(assertHook.assertFailed());

    dispatchAll();
    ASSERT_FALSE(assertHook.assertFailed());

    // Resend was issued once on recovery, then delivery succeeded
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_dataOut(1, buffer_a, context);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer_a, context);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success::SUCCESS);
    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, 0x1234);
    const OutputKind expected[] = {DATA_OUT, DATA_OUT, DATA_RETURN_OUT, COM_STATUS_OUT};
    checkOutputOrder(expected, FW_NUM_ARRAY_ELEMENTS(expected));
}

void ComRetryTester ::testSynchronousAdapterExhaustion() {
    U8 data_a[BUFFER_LENGTH] = DATA_A;
    Fw::Buffer buffer_a(&data_a[0], sizeof(data_a));
    ComCfg::FrameContext nullContext;
    const Fw::Success answers[] = {Fw::Success::FAILURE, Fw::Success::FAILURE};
    setInlineAdapter(answers, FW_NUM_ARRAY_ELEMENTS(answers));
    configure(1);

    invoke_to_dataIn(0, buffer_a, nullContext);
    dispatchAll();
    // First attempt failed: waiting for recovery, nothing returned upstream yet
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(0);
    ASSERT_from_comStatusOut_SIZE(0);

    // Recovery triggers the single retry, which also fails inline: retries are exhausted
    sendStatus(Fw::Success::SUCCESS);
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer_a, nullContext);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, Fw::Success::FAILURE);
    const OutputKind expected[] = {DATA_OUT, DATA_OUT, DATA_RETURN_OUT, COM_STATUS_OUT};
    checkOutputOrder(expected, FW_NUM_ARRAY_ELEMENTS(expected));

    // A subsequent recovery SUCCESS passes through with no buffer pending
    sendStatus(Fw::Success::SUCCESS);
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_comStatusOut_SIZE(2);
    ASSERT_from_comStatusOut(1, Fw::Success::SUCCESS);
}

void ComRetryTester ::testPing() {
    const U32 key = 0xdeadbeef;
    invoke_to_pingIn(0, key);
    ASSERT_from_pingOut_SIZE(0);
    dispatchAll();
    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, key);
}

void ComRetryTester ::testQueueFullAsserts() {
    ::Test::UnitTestAssert assertHook;
    Fw::Success success = Fw::Success::SUCCESS;
    for (FwSizeType i = 0; i < TEST_INSTANCE_QUEUE_DEPTH; i++) {
        invoke_to_comStatusIn(0, success);
    }
    ASSERT_FALSE(assertHook.assertFailed());

    // One message beyond the queue depth is a fatal assertion, not a silent drop
    invoke_to_comStatusIn(0, success);
    ASSERT_TRUE(assertHook.assertFailed());
    ::Test::UnitTestAssert::File file;
    FwSizeType lineNo = 0;
    FwSizeType numArgs = 0;
    FwAssertArgType arg1 = 0, arg2 = 0, arg3 = 0, arg4 = 0, arg5 = 0, arg6 = 0;
    assertHook.retrieveAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6);
    ASSERT_EQ(numArgs, 1);
    ASSERT_EQ(arg1, static_cast<FwAssertArgType>(Os::Queue::Status::FULL));
    assertHook.clearAssertFailure();

    dispatchAll();
    ASSERT_from_comStatusOut_SIZE(TEST_INSTANCE_QUEUE_DEPTH);
    ASSERT_FALSE(assertHook.assertFailed());
}

void ComRetryTester ::testPingDroppedWhenFull() {
    ::Test::UnitTestAssert assertHook;
    Fw::Success success = Fw::Success::SUCCESS;
    for (FwSizeType i = 0; i < TEST_INSTANCE_QUEUE_DEPTH; i++) {
        invoke_to_comStatusIn(0, success);
    }

    // A ping on a full queue is dropped rather than asserting
    invoke_to_pingIn(0, 1);
    ASSERT_FALSE(assertHook.assertFailed());
    dispatchAll();
    ASSERT_from_comStatusOut_SIZE(TEST_INSTANCE_QUEUE_DEPTH);
    ASSERT_from_pingOut_SIZE(0);

    // Pings are answered once space is available
    invoke_to_pingIn(0, 2);
    dispatchAll();
    ASSERT_from_pingOut_SIZE(1);
    ASSERT_from_pingOut(0, 2);
}

}  // namespace Svc
