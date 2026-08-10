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

ComAggregatorTester ::~ComAggregatorTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

Fw::Buffer ComAggregatorTester ::fill_buffer(U32 size) {
    EXPECT_GT(size, 0);
    U8* data = new U8[size];
    for (U32 i = 0; i < size; i++) {
        data[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 255));
    }
    Fw::Buffer buffer(data, static_cast<FwSizeType>(size));
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
    // Ensure we dispatched all messages
    ASSERT_EQ(this->component.m_queue.getMessagesAvailable(), 0);
}

//! Tests fill operation
Fw::Buffer ComAggregatorTester ::test_fill(bool expect_hold) {
    // Precondition: initial has run
    const FwSizeType ORIGINAL_LENGTH = this->component.m_frameSerializer.getSize();
    // Maximum size we can fill
    const FwSizeType MAX_FILL =
        ComCfg::AggregationSize - ORIGINAL_LENGTH - ((ORIGINAL_LENGTH == ComCfg::AggregationSize) ? 0 : 1);
    if (MAX_FILL == 0) {
        // Nothing to fill
        return Fw::Buffer();
    }
    // Allow a full buffer only in the case where we expect to hold
    const U32 BUFFER_LENGTH = STest::Pick::lowerUpper(1, static_cast<U32>(MAX_FILL));
    Fw::Buffer buffer = fill_buffer(BUFFER_LENGTH);
    ComCfg::FrameContext context;
    EXPECT_EQ(this->component.m_queue.getMessagesAvailable(), 0);
    this->invoke_to_dataIn(0, buffer, context);
    EXPECT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    EXPECT_EQ(this->component.m_queue.getMessagesAvailable(), 0);
    if (expect_hold) {
        EXPECT_EQ(ORIGINAL_LENGTH, this->component.m_frameSerializer.getSize());
    } else {
        EXPECT_EQ(ORIGINAL_LENGTH + BUFFER_LENGTH, this->component.m_frameSerializer.getSize());
        this->validate_buffer_aggregated(buffer, context);
    }
    EXPECT_EQ(this->component.m_queue.getMessagesAvailable(), 0);
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

//! Tests exactly full operation
void ComAggregatorTester ::test_exactly_full() {
    // Precondition: fill has run
    // Chose a buffer that will be too large to fit but still will fit after being aggregated
    const FwSizeType ORIGINAL_LENGTH = this->component.m_frameSerializer.getSize();
    const U32 BUFFER_LENGTH = static_cast<U32>(ComCfg::AggregationSize - ORIGINAL_LENGTH);
    Fw::Buffer buffer = fill_buffer(BUFFER_LENGTH);
    ComCfg::FrameContext context;

    // Send the overflow buffer and ensure the current aggregation comes out
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    // First validate the sent buffer was aggregated correctly. This also updates the shadow aggregation.
    this->validate_buffer_aggregated(buffer, context);
    // Now validate that the sent buffer matches the shadow aggregation.
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
    // Validate that there is no data aggregated
    ASSERT_EQ(this->component.m_frameSerializer.getSize(), 0);
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

//! Tests timeout operation
void ComAggregatorTester ::test_timeout_overflow_prevention() {
    ASSERT_EQ(this->component.m_queue.getMessagesAvailable(), 0);
    // Precondition: fill has run
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    this->validate_aggregation(this->fromPortHistory_dataOut->at(0).data);
    // Invoke some number of failure status. These prevent the timeout from being prematurely enabled.
    for (U32 i = 0; i < STest::Pick::lowerUpper(1, 5); i++) {
        Fw::Success bad = Fw::Success::FAILURE;
        this->invoke_to_comStatusIn(0, bad);
        ASSERT_EQ(this->dispatchOne(this->component),
                  Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
        // Should be no change
        this->validate_aggregation(this->component.m_frameBuffer);
        ASSERT_from_dataOut_SIZE(1);
    }
    // Now invoke enough extra timeouts to overflow the queue if they were all queued
    for (U32 i = 0; i < STest::Pick::lowerUpper(1, TEST_INSTANCE_QUEUE_DEPTH) + TEST_INSTANCE_QUEUE_DEPTH; i++) {
        // These timeouts should be dropped
        this->invoke_to_timeout(0, 0);
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

// ----------------------------------------------------------------------
// Spanning tests
// ----------------------------------------------------------------------

void ComAggregatorTester ::append_idle_packet(std::vector<U8>& expected, FwSizeType idleSize) {
    ASSERT_GE(idleSize, ComAggregator::MIN_IDLE_PACKET_SIZE);
    // packetIdentification: PVN 0, type TM, no secondary header, idle APID (all ones)
    expected.push_back(0x07);
    expected.push_back(0xFF);
    // packetSequenceControl: sequence flags 0b11 (unsegmented), count 0
    expected.push_back(0xC0);
    expected.push_back(0x00);
    // packetDataLength: number of payload bytes minus 1
    const U16 lengthToken = static_cast<U16>(idleSize - Ccsds::SpacePacketHeader::SERIALIZED_SIZE - 1);
    expected.push_back(static_cast<U8>(lengthToken >> 8));
    expected.push_back(static_cast<U8>(lengthToken & 0xFF));
    for (FwSizeType i = Ccsds::SpacePacketHeader::SERIALIZED_SIZE; i < idleSize; i++) {
        expected.push_back(ComAggregator::IDLE_DATA_PATTERN);
    }
}

void ComAggregatorTester ::spanning_send(Fw::Buffer& buffer) {
    ComCfg::FrameContext context;
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
}

void ComAggregatorTester ::expect_frame(U32 index, const std::vector<U8>& expected, U16 expectedFhp) {
    ASSERT_GT(this->fromPortHistory_dataOut->size(), index);
    const Fw::Buffer& frame = this->fromPortHistory_dataOut->at(index).data;
    const ComCfg::FrameContext& context = this->fromPortHistory_dataOut->at(index).context;
    // Spanning aggregates are always emitted at full capacity
    ASSERT_EQ(frame.getSize(), static_cast<FwSizeType>(ComCfg::AggregationSpanningSize));
    ASSERT_EQ(expected.size(), frame.getSize());
    for (FwSizeType i = 0; i < expected.size(); i++) {
        ASSERT_EQ(frame.getData()[i], expected[i]) << "Mismatch at frame offset " << i;
    }
    ASSERT_EQ(context.get_firstHeaderPointer(), expectedFhp);
}

void ComAggregatorTester ::return_and_status(U32 index) {
    // Const cast is safe as data is not altered
    this->invoke_to_dataReturnIn(0, const_cast<Fw::Buffer&>(this->fromPortHistory_dataOut->at(index).data),
                                 this->fromPortHistory_dataOut->at(index).context);
    Fw::Success good = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, good);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
}

void ComAggregatorTester ::test_spanning_split_two() {
    const FwSizeType CAP = ComCfg::AggregationSpanningSize;
    const FwSizeType FIRST_SIZE = 100;
    const FwSizeType SPAN_SIZE = CAP + 184;  // Overflows the first aggregate by 284 bytes
    const FwSizeType FRAME1_PORTION = CAP - FIRST_SIZE;
    const FwSizeType REMAINDER = SPAN_SIZE - FRAME1_PORTION;
    this->component.configure(true);
    this->test_initial();

    // Partially fill with a whole packet, then send the spanning packet
    Fw::Buffer first = this->fill_buffer(static_cast<U32>(FIRST_SIZE));
    this->spanning_send(first);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // Whole packet returned immediately
    Fw::Buffer span = this->fill_buffer(static_cast<U32>(SPAN_SIZE));
    this->spanning_send(span);

    // Frame 1: whole first packet + leading portion of the spanning packet; first header at offset 0
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);  // Spanning packet is retained, not returned
    std::vector<U8> expected1(first.getData(), first.getData() + FIRST_SIZE);
    expected1.insert(expected1.end(), span.getData(), span.getData() + FRAME1_PORTION);
    this->expect_frame(0, expected1, 0);
    this->return_and_status(0);
    ASSERT_from_dataReturnOut_SIZE(2);  // Remainder consumed: spanning packet returned

    // Frame 2 (via timeout): spanning packet remainder + idle fill; first header after the continuation
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(2);
    std::vector<U8> expected2(span.getData() + FRAME1_PORTION, span.getData() + SPAN_SIZE);
    append_idle_packet(expected2, CAP - REMAINDER);
    this->expect_frame(1, expected2, static_cast<U16>(REMAINDER));
    this->return_and_status(1);

    delete[] first.getData();
    delete[] span.getData();
    this->clearHistory();
}

void ComAggregatorTester ::test_spanning_three_frames() {
    const FwSizeType CAP = ComCfg::AggregationSpanningSize;
    const FwSizeType FIRST_SIZE = 200;
    const FwSizeType TAIL = 300;
    // Starts in frame 1, spans the complete frame 2, and ends in frame 3
    const FwSizeType SPAN_SIZE = (CAP - FIRST_SIZE) + CAP + TAIL;
    this->component.configure(true);
    this->test_initial();

    Fw::Buffer first = this->fill_buffer(static_cast<U32>(FIRST_SIZE));
    this->spanning_send(first);
    Fw::Buffer span = this->fill_buffer(static_cast<U32>(SPAN_SIZE));
    this->spanning_send(span);

    // Frame 1: whole first packet + leading portion of the spanning packet
    ASSERT_from_dataOut_SIZE(1);
    std::vector<U8> expected1(first.getData(), first.getData() + FIRST_SIZE);
    expected1.insert(expected1.end(), span.getData(), span.getData() + (CAP - FIRST_SIZE));
    this->expect_frame(0, expected1, 0);
    this->return_and_status(0);

    // Frame 2: sent immediately on good status; continuation data only (no packet header starts here)
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_dataReturnOut_SIZE(1);  // Spanning packet still retained
    std::vector<U8> expected2(span.getData() + (CAP - FIRST_SIZE), span.getData() + (2 * CAP - FIRST_SIZE));
    this->expect_frame(1, expected2, static_cast<U16>(Ccsds::TMSubfields::FHP_NO_PACKET_START));
    this->return_and_status(1);
    ASSERT_from_dataReturnOut_SIZE(2);  // Tail consumed: spanning packet returned

    // Frame 3 (via timeout): spanning packet tail + idle fill
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(3);
    std::vector<U8> expected3(span.getData() + (2 * CAP - FIRST_SIZE), span.getData() + SPAN_SIZE);
    append_idle_packet(expected3, CAP - TAIL);
    this->expect_frame(2, expected3, static_cast<U16>(TAIL));
    this->return_and_status(2);

    delete[] first.getData();
    delete[] span.getData();
    this->clearHistory();
}

void ComAggregatorTester ::test_spanning_idle_span() {
    const FwSizeType CAP = ComCfg::AggregationSpanningSize;
    const FwSizeType RESIDUAL = 3;  // Below the minimum idle packet size: idle packet must span
    const FwSizeType PACKET_SIZE = CAP - RESIDUAL;
    this->component.configure(true);
    this->test_initial();

    Fw::Buffer packet = this->fill_buffer(static_cast<U32>(PACKET_SIZE));
    this->spanning_send(packet);
    ASSERT_from_dataOut_SIZE(0);

    // Frame 1 (via timeout): packet + leading bytes of a minimum-size idle packet
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    std::vector<U8> idlePacket;
    append_idle_packet(idlePacket, ComAggregator::MIN_IDLE_PACKET_SIZE);
    std::vector<U8> expected1(packet.getData(), packet.getData() + PACKET_SIZE);
    expected1.insert(expected1.end(), idlePacket.begin(), idlePacket.begin() + static_cast<long>(RESIDUAL));
    this->expect_frame(0, expected1, 0);
    this->return_and_status(0);

    // Frame 2 (via timeout): trailing bytes of the spanned idle packet + a fresh idle fill
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(2);
    const FwSizeType CONTINUATION = ComAggregator::MIN_IDLE_PACKET_SIZE - RESIDUAL;
    std::vector<U8> expected2(idlePacket.begin() + static_cast<long>(RESIDUAL), idlePacket.end());
    append_idle_packet(expected2, CAP - CONTINUATION);
    this->expect_frame(1, expected2, static_cast<U16>(CONTINUATION));
    this->return_and_status(1);

    delete[] packet.getData();
    this->clearHistory();
}

}  // namespace Svc
