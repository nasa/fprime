// ======================================================================
// \title  ComAggregatorTester.cpp
// \author lestarch
// \brief  cpp file for ComAggregator component test harness implementation class
// ======================================================================

#include "ComAggregatorTester.hpp"
#include <STest/Pick/Pick.hpp>
#include <vector>
#include "config/FppConstantsAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ComAggregatorTester ::ComAggregatorTester()
    : ComAggregatorGTestBase("ComAggregatorTester", ComAggregatorTester::MAX_HISTORY_SIZE), component("ComAggregator") {
    this->initComponents();
    this->connectPorts();
}

ComAggregatorTester ::~ComAggregatorTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

Fw::Buffer ComAggregatorTester ::fill_buffer(U32 size) {
    EXPECT_GT(size, 0);
    U8* data = new U8[size];
    for (U32 i = 0; i < size; i++) {
        data[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 255));
    }
    Fw::Buffer buffer(data, size);
    return buffer;
}

//! Shadow aggregate a buffer for validation
void ComAggregatorTester ::shadow_aggregate(const Fw::Buffer& buffer) {
    for (FwSizeType i = 0; i < buffer.getSize(); i++) {
        this->m_aggregation.push_back(buffer.getData()[i]);
    }
}

//! Validate against shadow aggregation
void ComAggregatorTester ::validate_aggregation(const Fw::Buffer& buffer) {
    ASSERT_EQ(buffer.getSize(), this->m_aggregation.size());
    for (FwSizeType i = 0; i < this->m_aggregation.size(); i++) {
        ASSERT_EQ(buffer.getData()[i], this->m_aggregation[i]);
    }
}

void ComAggregatorTester ::validate_buffer_aggregated(const Fw::Buffer& buffer, const ComCfg::FrameContext& context) {
    FwSizeType start = this->component.m_frameSerializer.getSize() - buffer.getSize();
    for (FwSizeType i = 0; i < buffer.getSize(); i++) {
        ASSERT_EQ(buffer.getData()[i], this->component.m_frameBuffer.getData()[start + i]);
    }
    ASSERT_EQ(context, this->component.m_lastContext);
    this->shadow_aggregate(buffer);
    delete[] buffer.getData();
}

void ComAggregatorTester ::test_initial() {
    // Initial state should have empty buffer
    ASSERT_EQ(this->component.m_frameSerializer.getSize(), 0);
    ASSERT_EQ(this->component.m_bufferState, Fw::Buffer::OwnershipState::OWNED);
    this->component.preamble();
    ASSERT_from_comStatusOut(0, Fw::Success::SUCCESS);
    Fw::Success good = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, good);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
}

//! Tests fill operation
Fw::Buffer ComAggregatorTester ::test_fill(bool expect_hold) {
    // Precondition: initial has run
    const FwSizeType ORIGINAL_LENGTH = this->component.m_frameSerializer.getSize();
    if (ORIGINAL_LENGTH == ComCfg::AggregationSize) {
        // Nothing to fill
        return Fw::Buffer();
    }
    const U32 BUFFER_LENGTH = STest::Pick::lowerUpper(1, static_cast<U32>(ComCfg::AggregationSize - ORIGINAL_LENGTH));
    Fw::Buffer buffer = fill_buffer(BUFFER_LENGTH);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);
    EXPECT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    if (expect_hold) {
        EXPECT_EQ(ORIGINAL_LENGTH, this->component.m_frameSerializer.getSize());
    } else {
        EXPECT_EQ(ORIGINAL_LENGTH + BUFFER_LENGTH, this->component.m_frameSerializer.getSize());
        this->validate_buffer_aggregated(buffer, context);
    }
    this->clearHistory();
    return buffer;
}

void ComAggregatorTester ::test_fill_multi() {
    U32 count = STest::Pick::lowerUpper(1, 5);
    for (U32 i = 0; i < count; i++) {
        (void)this->test_fill();
    }
}

//! Tests full operation
void ComAggregatorTester ::test_full() {
    // Precondition: fill has run
    // Chose a buffer that will be too large to fit but still will fit after being aggregated
    const FwSizeType ORIGINAL_LENGTH = this->component.m_frameSerializer.getSize();
    const U32 BUFFER_LENGTH = STest::Pick::lowerUpper(static_cast<U32>(ComCfg::AggregationSize - ORIGINAL_LENGTH + 1),
                                                      static_cast<U32>(ComCfg::AggregationSize));
    Fw::Buffer buffer = fill_buffer(BUFFER_LENGTH);
    ComCfg::FrameContext context;

    // Send the overflow buffer and ensure the current aggregation comes out
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    this->validate_aggregation(this->fromPortHistory_dataOut->at(0).data);

    // Invoke some number of failures
    for (U32 i = 0; i < STest::Pick::lowerUpper(1, 5); i++) {
        Fw::Success bad = Fw::Success::FAILURE;
        this->invoke_to_comStatusIn(0, bad);
        ASSERT_EQ(this->dispatchOne(this->component),
                  Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
        // Should be no change
        this->validate_aggregation(this->component.m_frameBuffer);
        ASSERT_from_dataOut_SIZE(1);
    }
    // Const cast is safe as data is not altered
    this->invoke_to_dataReturnIn(0, const_cast<Fw::Buffer&>(this->fromPortHistory_dataOut->at(0).data),
                                 this->fromPortHistory_dataOut->at(0).context);
    Fw::Success good = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, good);
    this->m_aggregation.clear();
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    // Validate that the new buffer has been aggregated
    this->validate_buffer_aggregated(buffer, context);
    this->clearHistory();
}

//! Tests timeout operation
void ComAggregatorTester ::test_timeout() {
    // Precondition: fill has run
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    this->validate_aggregation(this->fromPortHistory_dataOut->at(0).data);

    // Invoke some number of failures
    for (U32 i = 0; i < STest::Pick::lowerUpper(1, 5); i++) {
        Fw::Success bad = Fw::Success::FAILURE;
        this->invoke_to_comStatusIn(0, bad);
        ASSERT_EQ(this->dispatchOne(this->component),
                  Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
        // Should be no change
        this->validate_aggregation(this->component.m_frameBuffer);
        ASSERT_from_dataOut_SIZE(1);
    }
    // Const cast is safe as data is not altered
    this->invoke_to_dataReturnIn(0, const_cast<Fw::Buffer&>(this->fromPortHistory_dataOut->at(0).data),
                                 this->fromPortHistory_dataOut->at(0).context);
    Fw::Success good = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, good);
    this->m_aggregation.clear();
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    this->clearHistory();
}

void ComAggregatorTester ::test_timeout_zero() {
    // Precondition: initialize has run
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();
}

//! Tests hold while waiting on data return
void ComAggregatorTester ::test_hold_while_waiting() {
    // Precondition: fill has run
    ComCfg::FrameContext context;
    this->invoke_to_timeout(0, 0);

    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    this->validate_aggregation(this->fromPortHistory_dataOut->at(0).data);
    Fw::Buffer major_buffer = this->fromPortHistory_dataOut->at(0).data;

    // Invoke some number of failures
    for (U32 i = 0; i < STest::Pick::lowerUpper(1, 5); i++) {
        Fw::Success bad = Fw::Success::FAILURE;
        this->invoke_to_comStatusIn(0, bad);
        ASSERT_EQ(this->dispatchOne(this->component),
                  Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
        // Should be no change
        this->validate_aggregation(this->component.m_frameBuffer);
        ASSERT_from_dataOut_SIZE(1);
    }
    // Force a hold
    Fw::Buffer minor_buffer = this->test_fill(true);

    // Const cast is safe as data is not altered
    this->invoke_to_dataReturnIn(0, major_buffer, context);
    Fw::Success good = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, good);
    this->m_aggregation.clear();
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    // Validate that the new buffer has been aggregated
    this->validate_buffer_aggregated(minor_buffer, context);
    this->clearHistory();
}

}  // namespace Svc
