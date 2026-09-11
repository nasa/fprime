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

constexpr FwSizeType ComAggregatorTester::DEFAULT_AGGREGATION_SIZE;
constexpr FwEnumStoreType ComAggregatorTester::TEST_ALLOCATION_ID;

// ----------------------------------------------------------------------
// Counting allocator
// ----------------------------------------------------------------------

void* CountingAllocator ::allocate(const FwEnumStoreType identifier,
                                   FwSizeType& size,
                                   bool& recoverable,
                                   FwSizeType alignment) {
    this->m_allocations++;
    this->m_lastId = identifier;
    if (this->m_failAllocation) {
        this->m_lastPointer = nullptr;
        return nullptr;
    }
    this->m_lastPointer = this->m_delegate.allocate(identifier, size, recoverable, alignment);
    size -= this->m_shortfall;
    return this->m_lastPointer;
}

void CountingAllocator ::deallocate(const FwEnumStoreType identifier, void* ptr) {
    this->m_deallocations++;
    this->m_lastId = identifier;
    EXPECT_EQ(ptr, this->m_lastPointer);
    this->m_delegate.deallocate(identifier, ptr);
}

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ComAggregatorTester ::ComAggregatorTester(FwSizeType aggregationSize, bool spanning)
    : ComAggregatorGTestBase("ComAggregatorTester", ComAggregatorTester::MAX_HISTORY_SIZE),
      component("ComAggregator"),
      m_allocator() {
    this->initComponents();
    this->connectPorts();
    this->component.configure(aggregationSize, spanning, TEST_ALLOCATION_ID, this->m_allocator);
    EXPECT_EQ(this->m_allocator.m_allocations, 1);
    EXPECT_EQ(this->m_allocator.m_lastId, TEST_ALLOCATION_ID);
    EXPECT_EQ(this->component.m_frameBuffer.getData(), static_cast<U8*>(this->m_allocator.m_lastPointer));
    EXPECT_EQ(this->aggregation_size(), aggregationSize);
}

ComAggregatorTester ::~ComAggregatorTester() {
    this->component.cleanup();
    EXPECT_EQ(this->m_allocator.m_deallocations, 1);
    EXPECT_EQ(this->m_allocator.m_lastId, TEST_ALLOCATION_ID);
    this->component.deinit();
}

FwSizeType ComAggregatorTester ::aggregation_size() const {
    return this->component.m_aggregationSize;
}

FwSizeType ComAggregatorTester ::max_packet_size() const {
    return this->aggregation_size() - Ccsds::Utils::IdlePacket::MIN_SIZE;
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
    // Without spanning the packets either fill the aggregate exactly or leave room for a whole idle packet
    std::vector<U8> expected = this->m_aggregation;
    ASSERT_LE(expected.size(), this->aggregation_size());
    if (expected.size() < this->aggregation_size()) {
        append_idle_packet(expected, this->aggregation_size() - expected.size());
    }
    ASSERT_EQ(buffer.getSize(), this->aggregation_size());
    ASSERT_EQ(buffer.getSize(), expected.size());
    for (FwSizeType i = 0; i < expected.size(); i++) {
        ASSERT_EQ(buffer.getData()[i], expected[i]) << "Mismatch at aggregate offset " << i;
    }
}

void ComAggregatorTester ::validate_emitted_aggregation(U32 index) {
    ASSERT_GT(this->fromPortHistory_dataOut->size(), index);
    this->validate_aggregation(this->fromPortHistory_dataOut->at(index).data);
    // Without spanning every aggregate starts with a packet header
    ASSERT_EQ(this->fromPortHistory_dataOut->at(index).context.get_firstHeaderPointer(), 0);
}

void ComAggregatorTester ::validate_buffer_aggregated(const Fw::Buffer& buffer, const ComCfg::FrameContext& context) {
    // The buffer follows the packets aggregated so far (the shadow), whether or not idle fill has been appended
    const FwSizeType start = this->m_aggregation.size();
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

void ComAggregatorTester ::fill_with(U32 size) {
    const FwSizeType ORIGINAL_LENGTH = this->component.m_frameSerializer.getSize();
    Fw::Buffer buffer = fill_buffer(size);
    ComCfg::FrameContext context;
    ASSERT_EQ(this->component.m_queue.getMessagesAvailable(), 0);
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_EQ(ORIGINAL_LENGTH + size, this->component.m_frameSerializer.getSize());
    this->validate_buffer_aggregated(buffer, context);
    this->clearHistory();
}

//! Tests fill operation
Fw::Buffer ComAggregatorTester ::test_fill(bool expect_hold) {
    // Precondition: initial has run
    const FwSizeType ORIGINAL_LENGTH = this->component.m_frameSerializer.getSize();
    // Maximum size we can fill while leaving room for a minimum idle packet: a held packet is filled into an empty
    // aggregate once the outstanding one returns
    const FwSizeType FILLED = expect_hold ? 0 : ORIGINAL_LENGTH;
    EXPECT_LE(FILLED, this->max_packet_size());
    const FwSizeType MAX_FILL = this->max_packet_size() - FILLED;
    if (MAX_FILL == 0) {
        // Nothing to fill
        return Fw::Buffer();
    }
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
    // Chose a buffer the current aggregate rejects (too large, or leaving less than a minimum idle packet of
    // residual) but that fits in an empty aggregate
    const FwSizeType ORIGINAL_LENGTH = this->component.m_frameSerializer.getSize();
    const FwSizeType REMAINING = this->aggregation_size() - ORIGINAL_LENGTH;
    const FwSizeType LOWER = (REMAINING > Ccsds::Utils::IdlePacket::MIN_SIZE - 1)
                                 ? (REMAINING - (Ccsds::Utils::IdlePacket::MIN_SIZE - 1))
                                 : 1;
    U32 BUFFER_LENGTH = STest::Pick::lowerUpper(static_cast<U32>(LOWER), static_cast<U32>(this->max_packet_size()));
    if (BUFFER_LENGTH == REMAINING) {
        // An exact fill is accepted: step off it in either direction, both of which are rejected
        BUFFER_LENGTH = (BUFFER_LENGTH == this->max_packet_size()) ? (BUFFER_LENGTH - 1) : (BUFFER_LENGTH + 1);
    }
    Fw::Buffer buffer = fill_buffer(BUFFER_LENGTH);
    ComCfg::FrameContext context;

    // Send the overflow buffer and ensure the current aggregation comes out
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    this->validate_emitted_aggregation(0);

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
    // Chose a buffer that exactly completes the aggregate: no idle packet is appended. The completing packet
    // must itself be acceptable, so top the aggregate up first when the remaining space exceeds the largest packet.
    if (this->aggregation_size() - this->component.m_frameSerializer.getSize() > this->max_packet_size()) {
        this->fill_with(static_cast<U32>(this->aggregation_size() - this->component.m_frameSerializer.getSize() -
                                         this->max_packet_size()));
    }
    const FwSizeType ORIGINAL_LENGTH = this->component.m_frameSerializer.getSize();
    const U32 BUFFER_LENGTH = static_cast<U32>(this->aggregation_size() - ORIGINAL_LENGTH);
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
    this->validate_emitted_aggregation(0);
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

void ComAggregatorTester ::test_small_residual_holds() {
    // Precondition: initial has run
    const FwSizeType FIRST_SIZE = 100;
    this->fill_with(static_cast<U32>(FIRST_SIZE));
    // Leave a residual too small for an idle packet: the packet is held and the aggregate goes out idle-filled
    const FwSizeType RESIDUAL = STest::Pick::lowerUpper(1, static_cast<U32>(Ccsds::Utils::IdlePacket::MIN_SIZE - 1));
    Fw::Buffer buffer = fill_buffer(static_cast<U32>(this->aggregation_size() - FIRST_SIZE - RESIDUAL));
    ComCfg::FrameContext context;
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(0);  // The held packet is not returned
    this->validate_emitted_aggregation(0);
    // Once the aggregate returns, the held packet starts the next one
    this->invoke_to_dataReturnIn(0, const_cast<Fw::Buffer&>(this->fromPortHistory_dataOut->at(0).data),
                                 this->fromPortHistory_dataOut->at(0).context);
    Fw::Success good = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, good);
    this->m_aggregation.clear();
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_EQ(this->component.m_frameSerializer.getSize(), buffer.getSize());
    ASSERT_from_dataReturnOut_SIZE(1);  // The held packet is returned once aggregated
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
    this->validate_emitted_aggregation(0);

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
    this->validate_emitted_aggregation(0);
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
    this->validate_emitted_aggregation(0);
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
    ASSERT_GE(idleSize, Ccsds::Utils::IdlePacket::MIN_SIZE);
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
        expected.push_back(Ccsds::Utils::IdlePacket::DATA_PATTERN);
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
    // Aggregates are always emitted at the configured aggregation size
    ASSERT_EQ(frame.getSize(), this->aggregation_size());
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
    // Precondition: constructed with spanning enabled
    const FwSizeType CAP = this->aggregation_size();
    const FwSizeType FIRST_SIZE = 100;
    const FwSizeType SPAN_SIZE = CAP + 184;  // Overflows the first aggregate by 284 bytes
    const FwSizeType FRAME1_PORTION = CAP - FIRST_SIZE;
    const FwSizeType REMAINDER = SPAN_SIZE - FRAME1_PORTION;
    this->test_initial();
    this->clearHistory();

    // Partially fill with a whole packet, then send the spanning packet
    Fw::Buffer first = this->fill_buffer(static_cast<U32>(FIRST_SIZE));
    this->spanning_send(first);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // Whole packet returned immediately
    ASSERT_from_comStatusOut_SIZE(1);
    Fw::Buffer span = this->fill_buffer(static_cast<U32>(SPAN_SIZE));
    this->spanning_send(span);

    // Frame 1: whole first packet + leading portion of the spanning packet; first header at offset 0
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);  // Spanning packet is retained, not returned
    ASSERT_from_comStatusOut_SIZE(1);
    std::vector<U8> expected1(first.getData(), first.getData() + FIRST_SIZE);
    expected1.insert(expected1.end(), span.getData(), span.getData() + FRAME1_PORTION);
    this->expect_frame(0, expected1, 0);
    this->return_and_status(0);
    ASSERT_from_dataReturnOut_SIZE(2);  // Remainder consumed: spanning packet returned
    ASSERT_from_comStatusOut_SIZE(2);

    // Frame 2 (via timeout): spanning packet remainder + idle fill; first header after the continuation
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(2);
    std::vector<U8> expected2(span.getData() + FRAME1_PORTION, span.getData() + SPAN_SIZE);
    append_idle_packet(expected2, CAP - REMAINDER);
    this->expect_frame(1, expected2, static_cast<U16>(REMAINDER));
    this->return_and_status(1);
    ASSERT_from_comStatusOut_SIZE(2);

    delete[] first.getData();
    delete[] span.getData();
    this->clearHistory();
}

void ComAggregatorTester ::test_spanning_three_frames() {
    // Precondition: constructed with spanning enabled
    const FwSizeType CAP = this->aggregation_size();
    const FwSizeType FIRST_SIZE = 200;
    const FwSizeType TAIL = 300;
    // Starts in frame 1, spans the complete frame 2, and ends in frame 3
    const FwSizeType SPAN_SIZE = (CAP - FIRST_SIZE) + CAP + TAIL;
    this->test_initial();
    this->clearHistory();

    Fw::Buffer first = this->fill_buffer(static_cast<U32>(FIRST_SIZE));
    this->spanning_send(first);
    ASSERT_from_comStatusOut_SIZE(1);
    Fw::Buffer span = this->fill_buffer(static_cast<U32>(SPAN_SIZE));
    this->spanning_send(span);

    // Frame 1: whole first packet + leading portion of the spanning packet
    ASSERT_from_dataOut_SIZE(1);
    std::vector<U8> expected1(first.getData(), first.getData() + FIRST_SIZE);
    expected1.insert(expected1.end(), span.getData(), span.getData() + (CAP - FIRST_SIZE));
    this->expect_frame(0, expected1, 0);
    this->return_and_status(0);
    ASSERT_from_comStatusOut_SIZE(1);

    // Frame 2: sent immediately on good status; continuation data only (no packet header starts here)
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_dataReturnOut_SIZE(1);  // Spanning packet still retained
    std::vector<U8> expected2(span.getData() + (CAP - FIRST_SIZE), span.getData() + (2 * CAP - FIRST_SIZE));
    this->expect_frame(1, expected2, static_cast<U16>(Ccsds::TMSubfields::FHP_NO_PACKET_START));
    this->return_and_status(1);
    ASSERT_from_dataReturnOut_SIZE(2);  // Tail consumed: spanning packet returned
    ASSERT_from_comStatusOut_SIZE(2);

    // Frame 3 (via timeout): spanning packet tail + idle fill
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(3);
    std::vector<U8> expected3(span.getData() + (2 * CAP - FIRST_SIZE), span.getData() + SPAN_SIZE);
    append_idle_packet(expected3, CAP - TAIL);
    this->expect_frame(2, expected3, static_cast<U16>(TAIL));
    this->return_and_status(2);
    ASSERT_from_comStatusOut_SIZE(2);

    delete[] first.getData();
    delete[] span.getData();
    this->clearHistory();
}

void ComAggregatorTester ::test_spanning_idle_span() {
    // Precondition: constructed with spanning enabled
    const FwSizeType CAP = this->aggregation_size();
    const FwSizeType RESIDUAL = 3;  // Below the minimum idle packet size: idle packet must span
    const FwSizeType PACKET_SIZE = CAP - RESIDUAL;
    this->test_initial();
    this->clearHistory();

    Fw::Buffer packet = this->fill_buffer(static_cast<U32>(PACKET_SIZE));
    this->spanning_send(packet);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_comStatusOut_SIZE(1);

    // Frame 1 (via timeout): packet + leading bytes of a minimum-size idle packet
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    std::vector<U8> idlePacket;
    append_idle_packet(idlePacket, Ccsds::Utils::IdlePacket::MIN_SIZE);
    std::vector<U8> expected1(packet.getData(), packet.getData() + PACKET_SIZE);
    expected1.insert(expected1.end(), idlePacket.begin(), idlePacket.begin() + static_cast<long>(RESIDUAL));
    this->expect_frame(0, expected1, 0);
    this->return_and_status(0);
    ASSERT_from_comStatusOut_SIZE(1);

    // Carried idle bytes alone do not trigger a timeout send
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);

    // Frame 2 (via timeout): trailing bytes of the spanned idle packet, a packet, and idle fill
    const FwSizeType SMALL_PACKET_SIZE = 10;
    Fw::Buffer smallPacket = this->fill_buffer(static_cast<U32>(SMALL_PACKET_SIZE));
    this->spanning_send(smallPacket);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(2);
    ASSERT_from_comStatusOut_SIZE(2);
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(2);
    const FwSizeType CONTINUATION = Ccsds::Utils::IdlePacket::MIN_SIZE - RESIDUAL;
    std::vector<U8> expected2(idlePacket.begin() + static_cast<long>(RESIDUAL), idlePacket.end());
    expected2.insert(expected2.end(), smallPacket.getData(), smallPacket.getData() + SMALL_PACKET_SIZE);
    append_idle_packet(expected2, CAP - CONTINUATION - SMALL_PACKET_SIZE);
    this->expect_frame(1, expected2, static_cast<U16>(CONTINUATION));
    this->return_and_status(1);
    ASSERT_from_comStatusOut_SIZE(2);

    delete[] packet.getData();
    delete[] smallPacket.getData();
    this->clearHistory();
}

// ----------------------------------------------------------------------
// Assertion tests
// ----------------------------------------------------------------------

void ComAggregatorTester ::test_reconfigure_without_cleanup_asserts() {
    ASSERT_DEATH_IF_SUPPORTED(
        this->component.configure(DEFAULT_AGGREGATION_SIZE, true, TEST_ALLOCATION_ID, this->m_allocator),
        "ComAggregator.cpp");
    this->clearHistory();
}

void ComAggregatorTester ::test_configure_invalid_size_asserts() {
    ComAggregator unconfigured("Unconfigured");
    CountingAllocator allocator;
    // Spanning or not, an aggregate must hold more than a minimum idle packet
    ASSERT_DEATH_IF_SUPPORTED(
        unconfigured.configure(Ccsds::Utils::IdlePacket::MIN_SIZE, true, TEST_ALLOCATION_ID, allocator),
        "ComAggregator.cpp");
    ASSERT_DEATH_IF_SUPPORTED(
        unconfigured.configure(Ccsds::Utils::IdlePacket::MIN_SIZE, false, TEST_ALLOCATION_ID, allocator),
        "ComAggregator.cpp");
    // Without spanning, a maximum-size Space Packet must fit next to a minimum idle packet
    ASSERT_DEATH_IF_SUPPORTED(unconfigured.configure(ComAggregator::MIN_NON_SPANNING_AGGREGATION_SIZE - 1, false,
                                                     TEST_ALLOCATION_ID, allocator),
                              "ComAggregator.cpp");
    // With spanning, every header offset must be representable as a First Header Pointer
    ASSERT_DEATH_IF_SUPPORTED(
        unconfigured.configure(static_cast<FwSizeType>(Ccsds::TMSubfields::FHP_IDLE_DATA_ONLY) + 1, true,
                               TEST_ALLOCATION_ID, allocator),
        "ComAggregator.cpp");
    // Without spanning, the largest residual (aggregationSize - 1) must be expressible as an idle packet
    ASSERT_DEATH_IF_SUPPORTED(
        unconfigured.configure(Ccsds::Utils::IdlePacket::MAX_SIZE + 2, false, TEST_ALLOCATION_ID, allocator),
        "ComAggregator.cpp");
    // Sizes at the bounds are accepted
    ComAggregator smallest("Smallest");
    smallest.configure(ComAggregator::MIN_NON_SPANNING_AGGREGATION_SIZE, false, TEST_ALLOCATION_ID, allocator);
    ASSERT_EQ(smallest.m_aggregationSize, ComAggregator::MIN_NON_SPANNING_AGGREGATION_SIZE);
    smallest.cleanup();
    ComAggregator largest("Largest");
    largest.configure(static_cast<FwSizeType>(Ccsds::TMSubfields::FHP_IDLE_DATA_ONLY), true, TEST_ALLOCATION_ID,
                      allocator);
    ASSERT_EQ(largest.m_aggregationSize, static_cast<FwSizeType>(Ccsds::TMSubfields::FHP_IDLE_DATA_ONLY));
    largest.cleanup();
    ComAggregator largestNonSpanning("LargestNonSpanning");
    largestNonSpanning.configure(Ccsds::Utils::IdlePacket::MAX_SIZE + 1, false, TEST_ALLOCATION_ID, allocator);
    ASSERT_EQ(largestNonSpanning.m_aggregationSize, Ccsds::Utils::IdlePacket::MAX_SIZE + 1);
    largestNonSpanning.cleanup();
    ASSERT_EQ(allocator.m_allocations, 3);
    ASSERT_EQ(allocator.m_deallocations, 3);
}

void ComAggregatorTester ::test_configure_allocation_failure_asserts() {
    ComAggregator unconfigured("Unconfigured");
    CountingAllocator failing;
    failing.m_failAllocation = true;
    ASSERT_DEATH_IF_SUPPORTED(unconfigured.configure(DEFAULT_AGGREGATION_SIZE, false, TEST_ALLOCATION_ID, failing),
                              "MemAllocator.cpp");
    CountingAllocator undersized;
    undersized.m_shortfall = 1;
    ASSERT_DEATH_IF_SUPPORTED(unconfigured.configure(DEFAULT_AGGREGATION_SIZE, false, TEST_ALLOCATION_ID, undersized),
                              "MemAllocator.cpp");
}

void ComAggregatorTester ::test_cleanup() {
    // cleanup() before configure() is a no-op
    ComAggregator unconfigured("Unconfigured");
    unconfigured.cleanup();
    // cleanup() releases the buffer once; repeated calls do not release it again
    ASSERT_EQ(this->m_allocator.m_allocations, 1);
    ASSERT_EQ(this->m_allocator.m_deallocations, 0);
    this->component.cleanup();
    ASSERT_EQ(this->m_allocator.m_deallocations, 1);
    ASSERT_EQ(this->component.m_allocation, nullptr);
    // The aggregate buffer no longer aliases the released storage
    ASSERT_EQ(this->component.m_frameBuffer.getData(), nullptr);
    ASSERT_EQ(this->component.m_frameBuffer.getSize(), 0);
    ASSERT_EQ(this->component.m_frameSerializer.getCapacity(), 0);
    ASSERT_EQ(this->component.m_aggregationSize, 0);
    this->component.cleanup();
    ASSERT_EQ(this->m_allocator.m_deallocations, 1);
    // cleanup() followed by configure() re-acquires storage
    ComAggregator reconfigured("Reconfigured");
    CountingAllocator allocator;
    reconfigured.configure(DEFAULT_AGGREGATION_SIZE, false, TEST_ALLOCATION_ID, allocator);
    reconfigured.cleanup();
    reconfigured.configure(DEFAULT_AGGREGATION_SIZE, true, TEST_ALLOCATION_ID, allocator);
    ASSERT_EQ(allocator.m_allocations, 2);
    ASSERT_EQ(allocator.m_deallocations, 1);
    ASSERT_EQ(reconfigured.m_frameBuffer.getData(), static_cast<U8*>(allocator.m_lastPointer));
    ASSERT_EQ(reconfigured.m_aggregationSize, DEFAULT_AGGREGATION_SIZE);
    reconfigured.cleanup();
    ASSERT_EQ(allocator.m_deallocations, 2);
}

void ComAggregatorTester ::test_datain_after_cleanup_asserts() {
    this->component.cleanup();
    ComCfg::FrameContext context;
    Fw::Buffer buffer = this->fill_buffer(1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "ComAggregator.cpp");
    delete[] buffer.getData();
}

void ComAggregatorTester ::test_cleanup_while_held_asserts() {
    // Precondition: fill has run
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->component.m_bufferState, Fw::Buffer::OwnershipState::NOT_OWNED);
    ASSERT_DEATH_IF_SUPPORTED(this->component.cleanup(), "ComAggregator.cpp");
    ASSERT_EQ(this->m_allocator.m_deallocations, 0);
    // Const cast is safe as data is not altered
    this->invoke_to_dataReturnIn(0, const_cast<Fw::Buffer&>(this->fromPortHistory_dataOut->at(0).data),
                                 this->fromPortHistory_dataOut->at(0).context);
    ASSERT_EQ(this->component.m_bufferState, Fw::Buffer::OwnershipState::OWNED);
    this->clearHistory();
}

void ComAggregatorTester ::test_oversize_hold_asserts() {
    // Precondition: fill has run, spanning disabled
    ComCfg::FrameContext context;
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component),
              Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);  // Dispatch the state machine
    ASSERT_from_dataOut_SIZE(1);

    // Hold a packet that can never fit in a single aggregate next to an idle packet
    Fw::Buffer oversize = this->fill_buffer(static_cast<U32>(this->max_packet_size()) + 1);
    ASSERT_DEATH_IF_SUPPORTED((this->invoke_to_dataIn(0, oversize, context), this->dispatchOne(this->component)),
                              "ComAggregator.cpp");
    delete[] oversize.getData();
    // Return the outstanding aggregate so the instance can be cleaned up
    // Const cast is safe as data is not altered
    this->invoke_to_dataReturnIn(0, const_cast<Fw::Buffer&>(this->fromPortHistory_dataOut->at(0).data),
                                 this->fromPortHistory_dataOut->at(0).context);
    this->clearHistory();
}

void ComAggregatorTester ::test_oversize_fill_asserts() {
    this->test_initial();
    ComCfg::FrameContext context;
    Fw::Buffer oversize = this->fill_buffer(static_cast<U32>(this->max_packet_size()) + 1);
    ASSERT_DEATH_IF_SUPPORTED((this->invoke_to_dataIn(0, oversize, context), this->dispatchOne(this->component)),
                              "ComAggregator.cpp");
    delete[] oversize.getData();
    this->clearHistory();
}

void ComAggregatorTester ::test_spanning_failure_drops_split_remainder() {
    // Precondition: constructed with spanning enabled
    const FwSizeType CAP = this->aggregation_size();
    const FwSizeType FIRST_SIZE = 100;
    const FwSizeType SPAN_SIZE = CAP + 184;
    this->test_initial();
    this->clearHistory();

    Fw::Buffer first = this->fill_buffer(static_cast<U32>(FIRST_SIZE));
    this->spanning_send(first);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    this->clearHistory();

    Fw::Buffer span = this->fill_buffer(static_cast<U32>(SPAN_SIZE));
    this->spanning_send(span);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(0);
    ASSERT_from_comStatusOut_SIZE(0);

    Fw::Buffer frame = this->fromPortHistory_dataOut->at(0).data;
    ComCfg::FrameContext frameContext = this->fromPortHistory_dataOut->at(0).context;
    this->clearHistory();
    this->invoke_to_dataReturnIn(0, frame, frameContext);
    Fw::Success bad = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, bad);
    ASSERT_EQ(this->dispatchOne(this->component), Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(0);
    ASSERT_from_comStatusOut_SIZE(0);

    Fw::Success good = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, good);
    ASSERT_EQ(this->dispatchOne(this->component), Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), span.getData());
    delete[] span.getData();
    this->clearHistory();

    const FwSizeType SMALL_PACKET_SIZE = 50;
    Fw::Buffer packet = this->fill_buffer(static_cast<U32>(SMALL_PACKET_SIZE));
    this->spanning_send(packet);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component), Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_from_dataOut_SIZE(1);
    std::vector<U8> expected(packet.getData(), packet.getData() + SMALL_PACKET_SIZE);
    append_idle_packet(expected, CAP - SMALL_PACKET_SIZE);
    this->expect_frame(0, expected, 0);
    this->return_and_status(0);

    delete[] first.getData();
    delete[] packet.getData();
    this->clearHistory();
}
}  // namespace Svc
