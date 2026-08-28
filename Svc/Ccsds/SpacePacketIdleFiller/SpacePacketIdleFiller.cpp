// ======================================================================
// \title  SpacePacketIdleFiller.cpp
// \author claradavisb
// \brief  cpp file for SpacePacketIdleFiller component implementation class
// ======================================================================

#include "Svc/Ccsds/SpacePacketIdleFiller/SpacePacketIdleFiller.hpp"
#include "Fw/Types/Assert.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

SpacePacketIdleFiller ::SpacePacketIdleFiller(const char* const compName)
    : SpacePacketIdleFillerComponentBase(compName),
      m_fillBuffer(),
      m_targetSize(0),
      m_configured(false),
      m_bufferState(BufferOwnershipState::OWNED) {}

SpacePacketIdleFiller ::~SpacePacketIdleFiller() {}

void SpacePacketIdleFiller ::configure(FwSizeType targetSize) {
    FW_ASSERT(targetSize > 0, static_cast<FwAssertArgType>(targetSize));
    FW_ASSERT(targetSize <= MAX_FILL_SIZE, static_cast<FwAssertArgType>(targetSize),
              static_cast<FwAssertArgType>(MAX_FILL_SIZE));
    this->m_targetSize = targetSize;
    this->m_configured = true;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void SpacePacketIdleFiller ::dataIn_handler(FwIndexType portNum,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_configured);
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED,
              static_cast<FwAssertArgType>(this->m_bufferState));

    const FwSizeType inputSize = static_cast<FwSizeType>(data.getSize());

    if (inputSize > this->m_targetSize) {
        this->log_WARNING_HI_InputTooLarge(inputSize, this->m_targetSize);
        this->dropBuffer(data, context);
        return;
    }
    const FwSizeType gap = this->m_targetSize - inputSize;
    if ((gap != 0u) && (gap < MIN_IDLE_PACKET_SIZE)) {
        this->log_WARNING_HI_GapTooSmall(gap);
        this->dropBuffer(data, context);
        return;
    }

    Fw::Buffer fillBuffer(this->m_fillBuffer, static_cast<Fw::Buffer::SizeType>(this->m_targetSize));
    auto serializer = fillBuffer.getSerializer();
    Fw::SerializeStatus status =
        serializer.serializeFrom(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

    // TM 132.0-B-3 sect. 4.2.2.5 calls for fill only where the data field would be short
    if (gap != 0u) {
        // Length token is the number of bytes of packet data minus one
        const U16 lengthToken = static_cast<U16>(gap - SpacePacketHeader::SERIALIZED_SIZE - 1);
        SpacePacketHeader idleHeader;
        idleHeader.set_packetIdentification(static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET));
        // Sequence flags 0b11 (unsegmented); the sequence count is unused for idle packets
        idleHeader.set_packetSequenceControl(
            static_cast<U16>(0x3 << SpacePacketSubfields::SeqFlagsOffset));
        idleHeader.set_packetDataLength(lengthToken);

        status = serializer.serializeFrom(idleHeader);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        for (FwSizeType i = SpacePacketHeader::SERIALIZED_SIZE; i < gap; i++) {
            status = serializer.serializeFrom(IDLE_DATA_PATTERN);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        }
    }

    this->m_bufferState = BufferOwnershipState::NOT_OWNED;
    this->dataOut_out(0, fillBuffer, context);
    this->dataReturnOut_out(0, data, context);  // the incoming buffer has been copied
}

void SpacePacketIdleFiller ::dataReturnIn_handler(FwIndexType portNum,
                                                  Fw::Buffer& data,
                                                  const ComCfg::FrameContext& context) {
    // Only component storage ever goes out on dataOut, so only it comes back here
    FW_ASSERT(data.getData() == &this->m_fillBuffer[0]);
    this->m_bufferState = BufferOwnershipState::OWNED;
}

void SpacePacketIdleFiller ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    this->comStatusOut_out(0, condition);
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

void SpacePacketIdleFiller ::dropBuffer(Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // Return the buffer before releasing the token: the status is what drives the upstream
    // aggregator to clear and refill the store this buffer points into.
    this->dataReturnOut_out(0, data, context);
    // SUCCESS means "this transmission opportunity is over, send the next one". FAILURE would
    // park the aggregator until a recovery status only a comm adapter can send, stalling the
    // downlink for good. The dropped frame is recorded by the event.
    Fw::Success status = Fw::Success::SUCCESS;
    if (this->isConnected_comStatusOut_OutputPort(0)) {
        this->comStatusOut_out(0, status);
    }
}

}  // namespace Ccsds

}  // namespace Svc
