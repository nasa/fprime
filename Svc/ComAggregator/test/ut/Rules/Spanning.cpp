// ======================================================================
// \title  Spanning.cpp
// \brief  Rule implementations for the Spanning rule group
//
// These rules drive the ComAggregator with packet spanning enabled against
// a shadow byte-stream model and validate every emitted aggregate.
// ======================================================================

#include <STest/Pick/Pick.hpp>
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/ComAggregator/test/ut/ComAggregatorTester.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

namespace {
constexpr FwSizeType CAPACITY = ComCfg::AggregationSpanningSize;
// Packets up to two aggregates plus change exercise start, middle, and end spans
constexpr U32 MAX_PACKET_SIZE = static_cast<U32>(2 * CAPACITY + 64);
}  // namespace

// ----------------------------------------------------------------------
// Scenario setup and teardown
// ----------------------------------------------------------------------

void ComAggregatorTester::spanning_rbt_start() {
    this->component.configure(true);
    this->test_initial();
}

void ComAggregatorTester::spanning_rbt_finish() {
    while (!this->m_unreturned.empty()) {
        delete[] this->m_unreturned.front();
        this->m_unreturned.pop_front();
    }
}

// ----------------------------------------------------------------------
// Shadow model helpers
// ----------------------------------------------------------------------

void ComAggregatorTester::shadow_send_packet() {
    const U32 size = STest::Pick::lowerUpper(1, MAX_PACKET_SIZE);
    Fw::Buffer packet = this->fill_buffer(size);
    this->m_unreturned.push_back(packet.getData());
    this->m_headers.push_back(this->m_stream.size());
    this->m_stream.insert(this->m_stream.end(), packet.getData(), packet.getData() + size);
    this->spanning_send(packet);
}

void ComAggregatorTester::shadow_emit() {
    ASSERT_from_dataOut_SIZE(1);
    const Fw::Buffer& frame = this->fromPortHistory_dataOut->at(0).data;
    const ComCfg::FrameContext& context = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_GE(this->m_stream.size(), CAPACITY);
    ASSERT_EQ(frame.getSize(), CAPACITY);
    for (FwSizeType i = 0; i < CAPACITY; i++) {
        ASSERT_EQ(frame.getData()[i], this->m_stream[i]) << "Mismatch at aggregate offset " << i;
    }
    // First Header Pointer: first packet header in this aggregate, or the continuation-only marker
    const bool hasHeader = !this->m_headers.empty() && (this->m_headers.front() < CAPACITY);
    const U16 expectedFhp = hasHeader ? static_cast<U16>(this->m_headers.front())
                                      : static_cast<U16>(Ccsds::TMSubfields::FHP_NO_PACKET_START);
    ASSERT_EQ(context.get_firstHeaderPointer(), expectedFhp);

    // Consume the emitted bytes from the shadow stream
    this->m_stream.erase(this->m_stream.begin(), this->m_stream.begin() + static_cast<long>(CAPACITY));
    std::vector<FwSizeType> remainingHeaders;
    for (FwSizeType offset : this->m_headers) {
        if (offset >= CAPACITY) {
            remainingHeaders.push_back(offset - CAPACITY);
        }
    }
    this->m_headers = remainingHeaders;
    this->m_outFrame = frame;
    this->m_outContext = context;
    this->m_outstanding = true;
}

void ComAggregatorTester::shadow_expect_return(U32 index) {
    ASSERT_GT(this->fromPortHistory_dataReturnOut->size(), index);
    ASSERT_FALSE(this->m_unreturned.empty());
    U8* expected = this->m_unreturned.front();
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(index).data.getData(), expected);
    this->m_unreturned.pop_front();
    delete[] expected;
}

// ----------------------------------------------------------------------
// Spanning.SendPacket
// ----------------------------------------------------------------------

bool ComAggregatorTester::Spanning__SendPacket__precondition() const {
    return !this->m_outstanding;
}

void ComAggregatorTester::Spanning__SendPacket__action() {
    this->clearHistory();
    this->shadow_send_packet();
    if (this->m_stream.size() < CAPACITY) {
        // Whole packet fits: aggregated and returned, nothing emitted
        ASSERT_from_dataOut_SIZE(0);
        ASSERT_from_dataReturnOut_SIZE(1);
        this->shadow_expect_return(0);
        ASSERT_EQ(this->component.m_frameSerializer.getSize(), this->m_stream.size());
    } else if (this->m_stream.size() == CAPACITY) {
        // Whole packet exactly fills: aggregated, returned, and emitted
        ASSERT_from_dataReturnOut_SIZE(1);
        this->shadow_expect_return(0);
        this->shadow_emit();
    } else {
        // Packet overflows: leading bytes emitted, the packet is retained for continuation
        ASSERT_from_dataReturnOut_SIZE(0);
        this->m_heldPending = true;
        this->shadow_emit();
    }
}

// ----------------------------------------------------------------------
// Spanning.SendPacketWhileWaiting
// ----------------------------------------------------------------------

bool ComAggregatorTester::Spanning__SendPacketWhileWaiting__precondition() const {
    // Upstream only sends after a good status, i.e. once the previous packet has been returned
    return this->m_outstanding && !this->m_heldPending;
}

void ComAggregatorTester::Spanning__SendPacketWhileWaiting__action() {
    this->clearHistory();
    this->shadow_send_packet();
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(0);
    this->m_heldPending = true;
}

// ----------------------------------------------------------------------
// Spanning.Timeout
// ----------------------------------------------------------------------

bool ComAggregatorTester::Spanning__Timeout__precondition() const {
    return !this->m_outstanding;
}

void ComAggregatorTester::Spanning__Timeout__action() {
    this->clearHistory();
    this->invoke_to_timeout(0, 0);
    ASSERT_EQ(this->dispatchOne(this->component), Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    if (this->m_stream.empty()) {
        // Nothing aggregated: no empty aggregate is emitted
        ASSERT_from_dataOut_SIZE(0);
        return;
    }
    // Residual space is filled by an idle packet, spanning into the next aggregate when too small
    const FwSizeType residual = CAPACITY - this->m_stream.size();
    const FwSizeType idleSize = FW_MAX(residual, Ccsds::Utils::IdlePacket::MIN_SIZE);
    this->m_headers.push_back(this->m_stream.size());
    append_idle_packet(this->m_stream, idleSize);
    ASSERT_from_dataReturnOut_SIZE(0);
    this->shadow_emit();
}

// ----------------------------------------------------------------------
// Spanning.StatusFailure
// ----------------------------------------------------------------------

bool ComAggregatorTester::Spanning__StatusFailure__precondition() const {
    return this->m_outstanding;
}

void ComAggregatorTester::Spanning__StatusFailure__action() {
    this->clearHistory();
    Fw::Success bad = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, bad);
    ASSERT_EQ(this->dispatchOne(this->component), Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(0);
}

// ----------------------------------------------------------------------
// Spanning.ReturnAndStatus
// ----------------------------------------------------------------------

bool ComAggregatorTester::Spanning__ReturnAndStatus__precondition() const {
    return this->m_outstanding;
}

void ComAggregatorTester::Spanning__ReturnAndStatus__action() {
    this->clearHistory();
    this->invoke_to_dataReturnIn(0, this->m_outFrame, this->m_outContext);
    Fw::Success good = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, good);
    ASSERT_EQ(this->dispatchOne(this->component), Svc::ComAggregatorComponentBase::MsgDispatchStatus::MSG_DISPATCH_OK);
    this->m_outstanding = false;

    // The held packet is always last in the stream: it is returned once it fits in the refilled aggregate
    if (this->m_heldPending && (this->m_stream.size() <= CAPACITY)) {
        ASSERT_from_dataReturnOut_SIZE(1);
        this->shadow_expect_return(0);
        this->m_heldPending = false;
    } else {
        ASSERT_from_dataReturnOut_SIZE(0);
    }
    if (this->m_stream.size() >= CAPACITY) {
        // Continuation data refilled a complete aggregate: emitted immediately
        this->shadow_emit();
    } else {
        ASSERT_from_dataOut_SIZE(0);
        ASSERT_EQ(this->component.m_frameSerializer.getSize(), this->m_stream.size());
    }
}

}  // namespace Svc
