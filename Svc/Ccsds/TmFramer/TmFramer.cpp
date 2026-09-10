// ======================================================================
// \title  TmFramer.cpp
// \author thomas-bc
// \brief  cpp file for TmFramer component implementation class
// ======================================================================

#include "Svc/Ccsds/TmFramer/TmFramer.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
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
    // The data must fill the data field exactly: idle filling (4.2.2.5) is done upstream by Svc::ComAggregator
    FW_ASSERT(data.getSize() == TmPayloadCapacity, static_cast<FwAssertArgType>(data.getSize()));
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
    // - all flags to 0 except Segment Length Id 0b11 (Standard 4.1.2.7.5)
    // - First Header Pointer from context (Standard 4.1.2.7.6): set by an upstream aggregator when packets
    //   span frames; the default of 0 indicates a packet header at offset 0 of the data field
    const U16 dataFieldStatus =
        static_cast<U16>((0x3 << TMSubfields::segLengthOffset) | context.get_firstHeaderPointer());

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
}  // namespace Ccsds
}  // namespace Svc
