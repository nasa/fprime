// ======================================================================
// \title  ComRetryTester.cpp
// \author valdaarhun
// \brief  cpp file for ComRetry test harness implementation class
// ======================================================================

#include "ComRetryTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ComRetryTester ::ComRetryTester()
    : ComRetryGTestBase("ComRetryTester", ComRetryTester::MAX_HISTORY_SIZE), component("ComRetry") {
    this->initComponents();
    this->connectPorts();
}

ComRetryTester ::~ComRetryTester() {}

void ComRetryTester ::configure(U32 num_retries=1) {
    component.configure(num_retries);
}

void ComRetryTester ::receiveBuffer(Fw::Buffer &buffer, ComCfg::FrameContext &context) {
    invoke_to_dataIn(0, buffer, context);
    invoke_to_dataReturnIn(0, buffer, context);
}

void ComRetryTester ::checkDataOut(FwIndexType expectedIndex, U8* expectedData, FwSizeType expectedDataSize) {
    Fw::Buffer emittedBuffer = this->fromPortHistory_dataOut->at(expectedIndex).data;
    ASSERT_EQ(expectedDataSize, emittedBuffer.getSize());
    for (FwSizeType i = 0; i < expectedDataSize; i++) {
        ASSERT_EQ(emittedBuffer.getData()[i], expectedData[i]);
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ComRetryTester ::testBufferSend() {
    U8 data_a[BUFFER_LENGTH] = DATA_A;
    U8 data_b[BUFFER_LENGTH] = DATA_B;
    Fw::Buffer buffer_a(&data_a[0], sizeof(data_a));
    Fw::Buffer buffer_b(&data_b[0], sizeof(data_b));
    ComCfg::FrameContext nullContext;
    Fw::Success state = Fw::Success::SUCCESS;
    configure();

    receiveBuffer(buffer_a, nullContext);
    invoke_to_comStatusIn(0, state);
    ASSERT_from_dataReturnOut(0, buffer_a, nullContext);
    ASSERT_from_comStatusOut(0, state);

    receiveBuffer(buffer_b, nullContext);
    invoke_to_comStatusIn(0, state);
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
    invoke_to_comStatusIn(0, state);
    invoke_to_dataReturnIn(0, buffer_a, nullContext);

    state = Fw::Success::SUCCESS;
    invoke_to_comStatusIn(0, state);
    ASSERT_from_dataReturnOut(0, buffer_a, nullContext);
    ASSERT_from_comStatusOut(0, state);

    receiveBuffer(buffer_b, nullContext);
    invoke_to_comStatusIn(0, state);
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
    Fw::Success state = Fw::Success::FAILURE;

    FwIndexType num_retries = 3;
    configure(num_retries);

    receiveBuffer(buffer_a, nullContext);
    invoke_to_comStatusIn(0, state);
    checkDataOut(0, buffer_a.getData(), buffer_a.getSize());

    for (FwIndexType i = 1; i <= num_retries; i++) {
        invoke_to_dataReturnIn(0, buffer_a, nullContext);
        invoke_to_comStatusIn(0, state);
        checkDataOut(i, buffer_a.getData(), buffer_a.getSize());
    }

    ASSERT_from_dataReturnOut(0, buffer_a, nullContext);
    ASSERT_from_comStatusOut(0, state);

    state = Fw::Success::SUCCESS;
    receiveBuffer(buffer_b, nullContext);
    invoke_to_comStatusIn(0, state);
    ASSERT_from_dataReturnOut(1, buffer_b, nullContext);
    ASSERT_from_comStatusOut(1, state);
    checkDataOut(num_retries + 1, buffer_b.getData(), buffer_b.getSize());
}

}  // namespace Svc
