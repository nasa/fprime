// ======================================================================
// \title  ComAggregator.cpp
// \author lestarch
// \brief  cpp file for ComAggregator component implementation class
// ======================================================================

#include "Svc/ComAggregator/ComAggregator.hpp"
#include <cstring>
#include "Fw/FPrimeBasicTypes.hpp"

namespace Svc {

// Definitions for ODR-use of static constexpr members (required until C++17)
constexpr U16 ComAggregator::FHP_UNSET;
constexpr U8 ComAggregator::IDLE_DATA_PATTERN;
constexpr FwSizeType ComAggregator::MIN_IDLE_PACKET_SIZE;

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ComAggregator ::ComAggregator(const char* const compName)
    : ComAggregatorComponentBase(compName),
      m_bufferState(Fw::Buffer::OwnershipState::OWNED),
      m_frameBuffer(m_frameBufferStore, sizeof(m_frameBufferStore)),
      m_frameSerializer(m_frameBuffer.getSerializer()),
      m_allow_timeout(false),
      m_spanning(false),
      m_capacity(ComCfg::AggregationSize),
      m_heldOffset(0),
      m_fhp(FHP_UNSET),
      m_pendingIdleCount(0) {}

ComAggregator ::~ComAggregator() {}

void ComAggregator ::configure(bool spanningEnabled) {
    // Configuration must happen before any data is aggregated
    FW_ASSERT(this->m_frameSerializer.getSize() == 0, static_cast<FwAssertArgType>(this->m_frameSerializer.getSize()));
    this->m_spanning = spanningEnabled;
    this->m_capacity = spanningEnabled ? static_cast<FwSizeType>(ComCfg::AggregationSpanningSize)
                                       : static_cast<FwSizeType>(ComCfg::AggregationSize);
}

void ComAggregator ::preamble() {
    Fw::Success good = Fw::Success::SUCCESS;
    this->comStatusOut_out(0, good);
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ComAggregator ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    this->aggregationMachine_sendSignal_status(condition);
}

void ComAggregator ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    Svc::ComDataContextPair pair(data, context);
    this->aggregationMachine_sendSignal_fill(pair);
}

void ComAggregator ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::NOT_OWNED);
    this->m_bufferState = Fw::Buffer::OwnershipState::OWNED;
}

void ComAggregator ::timeout_handler(FwIndexType portNum, U32 context) {
    // Timeout is ignored in WAIT_STATUS state. However, the queue may not process timeout messages until the wait
    // status is returned because the port chain may be synchronous and downstream components (radio, retry, etc) may
    // take a long time to complete the transmission of data. This can cause the queue to overflow with messages that
    // will soon be discarded.
    //
    // Therefore, to fix the risk of queue overflow we only queue timeout messages when they would be processed by the
    // state machine (i.e. in the FILL state). Otherwise, these messages are not queued.
    //
    // Behaviorally, this solution will work exactly like the naive implementation with an infinite queue depth, but
    // prevents queue overflow when using finite queues.
    if (this->m_allow_timeout) {
        this->aggregationMachine_sendSignal_timeout();
    }
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void ComAggregator ::Svc_AggregationMachine_action_doClear(SmId smId, Svc_AggregationMachine::Signal signal) {
    this->m_allow_timeout = true;  // Allow timeout messages in FILL state
    this->m_frameSerializer.resetSer();
    this->m_frameBuffer.setSize(sizeof(this->m_frameBufferStore));
    this->m_lastContext = ComCfg::FrameContext();
    this->m_fhp = FHP_UNSET;
    // Write out any idle packet bytes spanning over from the previous aggregate
    if (this->m_pendingIdleCount > 0) {
        Fw::SerializeStatus status = this->m_frameSerializer.serializeFrom(
            this->m_pendingIdle, this->m_pendingIdleCount, Fw::Serialization::OMIT_LENGTH);
        FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
        this->m_pendingIdleCount = 0;
    }
    // Fill from the held buffer (whole packet, or remainder of a spanned packet)
    this->fillFromHeld();
}

void ComAggregator ::Svc_AggregationMachine_action_doFill(SmId smId,
                                                          Svc_AggregationMachine::Signal signal,
                                                          const Svc::ComDataContextPair& value) {
    this->markFirstHeaderIfUnset();
    Fw::SerializeStatus status = this->m_frameSerializer.serializeFrom(
        value.get_data().getData(), value.get_data().getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
    this->m_lastContext = value.get_context();
    Fw::Success good = Fw::Success::SUCCESS;
    // Return port does not alter data and thus const-cast is safe
    this->dataReturnOut_out(0, const_cast<Fw::Buffer&>(value.get_data()), value.get_context());
    this->comStatusOut_out(0, good);
}

void ComAggregator ::Svc_AggregationMachine_action_doSend(SmId smId, Svc_AggregationMachine::Signal signal) {
    // Send only when the buffer will be valid
    if (this->m_frameSerializer.getSize() > 0) {
        if (this->m_spanning) {
            this->fillResidualWithIdle();
            this->m_lastContext.set_firstHeaderPointer(
                (this->m_fhp == FHP_UNSET) ? static_cast<U16>(Ccsds::TMSubfields::FHP_NO_PACKET_START) : this->m_fhp);
        }
        this->m_bufferState = Fw::Buffer::OwnershipState::NOT_OWNED;
        this->m_frameBuffer.setSize(this->m_frameSerializer.getSize());
        this->m_allow_timeout = false;  // Timeout messages should be discarded in WAIT_STATUS state
        this->dataOut_out(0, this->m_frameBuffer, this->m_lastContext);
    }
}

void ComAggregator ::Svc_AggregationMachine_action_doHold(SmId smId,
                                                          Svc_AggregationMachine::Signal signal,
                                                          const Svc::ComDataContextPair& value) {
    FW_ASSERT(not this->m_held.get_data().isValid());
    this->m_held = value;
    this->m_heldOffset = 0;
}

void ComAggregator ::Svc_AggregationMachine_action_doSplitHold(SmId smId,
                                                               Svc_AggregationMachine::Signal signal,
                                                               const Svc::ComDataContextPair& value) {
    this->Svc_AggregationMachine_action_doHold(smId, signal, value);
    if (this->m_spanning) {
        // Split the leading bytes of the held packet into the remaining aggregation space
        const FwSizeType remaining = this->remainingCapacity();
        if (remaining > 0) {
            // The held packet's header starts at the current fill offset of this aggregate
            this->markFirstHeaderIfUnset();
            Fw::SerializeStatus status = this->m_frameSerializer.serializeFrom(value.get_data().getData(), remaining,
                                                                               Fw::Serialization::OMIT_LENGTH);
            FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
            this->m_heldOffset = remaining;
            this->m_lastContext = value.get_context();
        }
    }
}

void ComAggregator ::Svc_AggregationMachine_action_assertNoStatus(SmId smId, Svc_AggregationMachine::Signal signal) {
    // Status is not possible in this state, confirm by assertion
    FW_ASSERT(false);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool ComAggregator ::Svc_AggregationMachine_guard_isFull(SmId smId,
                                                         Svc_AggregationMachine::Signal signal,
                                                         const Svc::ComDataContextPair& value) const {
    // Without spanning, packets larger than the aggregation capacity can never be sent
    FW_ASSERT(this->m_spanning || value.get_data().getSize() <= this->m_capacity,
              static_cast<FwAssertArgType>(value.get_data().getSize()));
    return (this->remainingCapacity() < value.get_data().getSize());
}

bool ComAggregator ::Svc_AggregationMachine_guard_willFill(SmId smId,
                                                           Svc_AggregationMachine::Signal signal,
                                                           const Svc::ComDataContextPair& value) const {
    return (this->remainingCapacity() == value.get_data().getSize());
}

bool ComAggregator ::Svc_AggregationMachine_guard_isNotEmpty(SmId smId, Svc_AggregationMachine::Signal signal) const {
    return this->m_frameSerializer.getSize() > 0;
}

bool ComAggregator ::Svc_AggregationMachine_guard_isGood(SmId smId,
                                                         Svc_AggregationMachine::Signal signal,
                                                         const Fw::Success& value) const {
    return value == Fw::Success::SUCCESS;
}

bool ComAggregator ::Svc_AggregationMachine_guard_isSpanFull(SmId smId, Svc_AggregationMachine::Signal signal) const {
    return this->m_spanning && (this->m_frameSerializer.getSize() == this->m_capacity);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

FwSizeType ComAggregator ::remainingCapacity() const {
    FW_ASSERT(this->m_frameSerializer.getSize() <= this->m_capacity,
              static_cast<FwAssertArgType>(this->m_frameSerializer.getSize()));
    return this->m_capacity - this->m_frameSerializer.getSize();
}

void ComAggregator ::markFirstHeaderIfUnset() {
    if (this->m_spanning && this->m_fhp == FHP_UNSET) {
        this->m_fhp = static_cast<U16>(this->m_frameSerializer.getSize());
    }
}

void ComAggregator ::fillFromHeld() {
    if (this->m_held.get_data().isValid()) {
        const Fw::Buffer& held = this->m_held.get_data();
        const FwSizeType heldRemaining = held.getSize() - this->m_heldOffset;
        const FwSizeType fillSize = FW_MIN(this->remainingCapacity(), heldRemaining);
        if (this->m_heldOffset == 0) {
            // The held packet's header starts at the current fill offset of this aggregate
            this->markFirstHeaderIfUnset();
        }
        Fw::SerializeStatus status = this->m_frameSerializer.serializeFrom(held.getData() + this->m_heldOffset,
                                                                           fillSize, Fw::Serialization::OMIT_LENGTH);
        FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
        this->m_lastContext = this->m_held.get_context();
        this->m_heldOffset += fillSize;
        if (this->m_heldOffset == held.getSize()) {
            // Held buffer fully consumed: return it and request more data
            Fw::Success good = Fw::Success::SUCCESS;
            // Return port does not alter data and thus const-cast is safe
            this->dataReturnOut_out(0, const_cast<Fw::Buffer&>(this->m_held.get_data()), this->m_held.get_context());
            this->comStatusOut_out(0, good);
            this->m_held = Svc::ComDataContextPair();
            this->m_heldOffset = 0;
        }
    }
}

void ComAggregator ::fillResidualWithIdle() {
    const FwSizeType residual = this->remainingCapacity();
    if (residual == 0) {
        return;
    }
    // The idle packet's header starts at the current fill offset of this aggregate
    this->markFirstHeaderIfUnset();
    // Idle packet size: fill the residual space exactly, spanning a minimum-size idle packet
    // into the next aggregate when the residual space is too small (CCSDS 132.0-B-3 4.1.4)
    const FwSizeType idleSize = FW_MAX(residual, MIN_IDLE_PACKET_SIZE);
    Ccsds::SpacePacketHeader header;
    header.set_packetIdentification(static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET));
    // Sequence Flags = 0b11 (unsegmented) & unused sequence count
    header.set_packetSequenceControl(static_cast<U16>(0x3 << Ccsds::SpacePacketSubfields::SeqFlagsOffset));
    // Length token is defined as the number of bytes of payload data minus 1
    header.set_packetDataLength(static_cast<U16>(idleSize - Ccsds::SpacePacketHeader::SERIALIZED_SIZE - 1));
    Fw::SerializeStatus status;
    if (residual >= MIN_IDLE_PACKET_SIZE) {
        // Idle packet fits entirely within this aggregate
        status = this->m_frameSerializer.serializeFrom(header);
        FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
        for (FwSizeType i = Ccsds::SpacePacketHeader::SERIALIZED_SIZE; i < idleSize; i++) {
            status = this->m_frameSerializer.serializeFrom(IDLE_DATA_PATTERN);
            FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
        }
    } else {
        // Stage a minimum-size idle packet, emit the leading bytes now and span the rest
        U8 staging[MIN_IDLE_PACKET_SIZE];
        Fw::ExternalSerializeBuffer stager(staging, sizeof(staging));
        status = stager.serializeFrom(header);
        FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
        staging[Ccsds::SpacePacketHeader::SERIALIZED_SIZE] = IDLE_DATA_PATTERN;
        status = this->m_frameSerializer.serializeFrom(staging, residual, Fw::Serialization::OMIT_LENGTH);
        FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK);
        this->m_pendingIdleCount = MIN_IDLE_PACKET_SIZE - residual;
        (void)memcpy(this->m_pendingIdle, &staging[residual], this->m_pendingIdleCount);
    }
}

}  // namespace Svc
