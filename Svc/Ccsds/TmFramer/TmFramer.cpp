// ======================================================================
// \title  TmFramer.cpp
// \author thomas-bc
// \brief  cpp file for TmFramer component implementation class
// ======================================================================

#include "Svc/Ccsds/TmFramer/TmFramer.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "Svc/Ccsds/Utils/IdlePacket.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

TmFramer ::TmFramer(const char* const compName)
    : TmFramerComponentBase(compName), m_masterFrameCount(0), m_virtualFrameCount(0) {}

TmFramer ::~TmFramer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void TmFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    FW_ASSERT(data.getSize() <= TmPayloadCapacity, static_cast<FwAssertArgType>(data.getSize()));
    // The data must either fill the data field exactly or leave room for a minimum idle packet (4.2.2.5)
    const FwSizeType residual = TmPayloadCapacity - data.getSize();
    FW_ASSERT(residual == 0 || residual >= Utils::IdlePacket::MIN_SIZE, static_cast<FwAssertArgType>(residual));
    FW_ASSERT(context.get_firstHeaderPointer() <= TMSubfields::fhpMask,
              static_cast<FwAssertArgType>(context.get_firstHeaderPointer()));
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED, static_cast<FwAssertArgType>(this->m_bufferState));

    // -----------------------------------------------
    // Header
    // -----------------------------------------------
    TMHeader header;

    // GVCID (Global Virtual Channel ID) (Standard 4.1.2.2 and 4.1.2.3)
    U16 globalVcId = static_cast<U16>(context.get_vcId() << TMSubfields::virtualChannelIdOffset);
    globalVcId |= static_cast<U16>(ComCfg::SpacecraftId << TMSubfields::spacecraftIdOffset);
    globalVcId |= 0x0;  // Operational Control Field: Flag set to 0 (Standard 4.1.2.4)

    // Data Field Status (Standard 4.1.2.7):
    // - all flags to 0 except segment length id 0b11 per standard (4.1.2.7)
    // - First Header Pointer from context (Standard 4.1.2.7.6): set by an upstream aggregator when packets
    //   span frames; the default of 0 indicates a packet header at offset 0 of the data field
    U16 dataFieldStatus = 0;
    dataFieldStatus |= 0x3 << TMSubfields::segLengthOffset;  // Seg Length Id '11' (0x3) per Standard (4.1.2.7.5)
    dataFieldStatus |= context.get_firstHeaderPointer();

    header.set_globalVcId(globalVcId);
    header.set_masterFrameCount(this->m_masterFrameCount);
    header.set_virtualFrameCount(this->m_virtualFrameCount);
    header.set_dataFieldStatus(dataFieldStatus);

    // We use only a single Virtual Channel for now, so master and virtual frame counts are the same
    this->m_masterFrameCount++;   // U8 intended to wrap around (modulo 256)
    this->m_virtualFrameCount++;  // U8 intended to wrap around (modulo 256)

    // -------------------------------------------------
    // Data field
    // -------------------------------------------------
    // Payload packet
    Fw::SerializeStatus status;
    // Create frame Fw::Buffer using member data field
    Fw::Buffer frameBuffer = Fw::Buffer(this->m_frameBuffer, sizeof(this->m_frameBuffer));
    auto frameSerializer = frameBuffer.getSerializer();
    status = frameSerializer.serializeFrom(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = frameSerializer.serializeFrom(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // As per TM Standard 4.2.2.5, fill the rest of the data field with an Idle Packet.
    // A full data field (e.g. delivered by a spanning aggregator) requires no fill.
    if (residual > 0) {
        this->fill_with_idle_packet(frameSerializer);
    }

    // -------------------------------------------------
    // Trailer (CRC)
    // -------------------------------------------------
    TMTrailer trailer;
    // Compute CRC over the entire frame buffer minus the FECF trailer (Standard 4.1.6)
    U16 crc =
        Ccsds::Utils::CRC16::compute(frameBuffer.getData(), sizeof(this->m_frameBuffer) - TMTrailer::SERIALIZED_SIZE);
    // Set the Frame Error Control Field (FECF)
    trailer.set_fecf(crc);
    // Move the serializer pointer to the end of the location where the trailer will be serialized
    status = frameSerializer.moveSerToOffset(ComCfg::TmFrameFixedSize - TMTrailer::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = frameSerializer.serializeFrom(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    this->m_bufferState = BufferOwnershipState::NOT_OWNED;
    this->dataOut_out(0, frameBuffer, context);
    this->dataReturnOut_out(0, data, context);  // return ownership of the original data buffer
}

void TmFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void TmFramer ::dataReturnIn_handler(FwIndexType portNum,
                                     Fw::Buffer& frameBuffer,
                                     const ComCfg::FrameContext& context) {
    // Assert that the returned buffer is the member, and set ownership state
    FW_ASSERT(frameBuffer.getData() >= &this->m_frameBuffer[0]);
    FW_ASSERT(frameBuffer.getData() < &this->m_frameBuffer[0] + sizeof(this->m_frameBuffer));
    this->m_bufferState = BufferOwnershipState::OWNED;
}

void TmFramer ::fill_with_idle_packet(Fw::SerialBufferBase& serializer) {
    constexpr FwSizeType endIndex = ComCfg::TmFrameFixedSize - TMTrailer::SERIALIZED_SIZE;
    const FwSizeType startIndex = serializer.getSize();
    FW_ASSERT(startIndex <= endIndex, static_cast<FwAssertArgType>(startIndex));
    const FwSizeType idlePacketSize = endIndex - startIndex;

    FW_ASSERT(idlePacketSize >= Utils::IdlePacket::MIN_SIZE, static_cast<FwAssertArgType>(idlePacketSize));
    FW_ASSERT(idlePacketSize <= ComCfg::TmFrameFixedSize, static_cast<FwAssertArgType>(idlePacketSize));

    const Fw::SerializeStatus status = Utils::IdlePacket::serialize(serializer, idlePacketSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
}
}  // namespace Ccsds
}  // namespace Svc
