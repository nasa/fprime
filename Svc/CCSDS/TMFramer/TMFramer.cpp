// ======================================================================
// \title  TMFramer.cpp
// \author thomas-bc
// \brief  cpp file for TMFramer component implementation class
// ======================================================================

#include "Svc/CCSDS/TMFramer/TMFramer.hpp"
#include "Svc/CCSDS/Types/FppConstantsAc.hpp"
#include "Svc/CCSDS/Types/TMHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/TMTrailerSerializableAc.hpp"
#include "Svc/CCSDS/Utils/CRC16.hpp"
#include "config/FppConstantsAc.hpp"
#include "Svc/CCSDS/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace CCSDS {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

TMFramer ::TMFramer(const char* const compName)
    : TMFramerComponentBase(compName), m_masterFrameCount(0), m_virtualFrameCount(0) {}

TMFramer ::~TMFramer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void TMFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // TODO: make this an event probably
    FW_ASSERT(data.getSize() <= ComCfg::TmFrameFixedSize - TMHeader::SERIALIZED_SIZE - TMTrailer::SERIALIZED_SIZE,
              static_cast<FwAssertArgType>(data.getSize()));

    // -----------------------------------------------
    // Header
    // -----------------------------------------------
    TMHeader header;

    U16 globalVcId = 0;
    globalVcId |= ComCfg::SpacecraftId << TMSubfields::spacecraftIdOffset;
    globalVcId |= static_cast<U16>(context.getvcId() << TMSubfields::virtualChannelIdOffset);
    globalVcId |= 0x0;  // Operational Control Field: Flag set to 0 (Standard 4.1.2.4)

    // Data Field Status (Standard 4.1.2.7):
    // - all flags to 0 except segment length id 0b11 per standard (4.1.2.7)
    // - First Header Pointer is always 0 since we are always wrapping a single entire packet at offset 0
    U16 dataFieldStatus = 0;
    dataFieldStatus |= 0x3 << TMSubfields::segLengthOffset;  // Seg Length Id 0b11 per Standard (4.1.2.7.5)

    header.setglobalVcId(globalVcId);
    header.setmasterFrameCount(this->m_masterFrameCount);
    header.setvirtualFrameCount(this->m_virtualFrameCount);
    header.setdataFieldStatus(dataFieldStatus);

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
    status = frameSerializer.serialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = frameSerializer.serialize(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // As per TM Standard 4.2.2.5, fill the rest of the data field with an Idle Packet
    this->fill_with_idle_packet(frameSerializer);

    // -------------------------------------------------
    // Trailer (CRC)
    // -------------------------------------------------
    TMTrailer trailer;
    // Compute CRC over the entire frame buffer minus the FECF trailer (Standard 4.1.6)
    U16 crc = CCSDS::Utils::CRC16::compute(frameBuffer.getData(),
                                           sizeof(this->m_frameBuffer) - TMTrailer::SERIALIZED_SIZE);
    // Set the Frame Error Control Field (FECF)
    trailer.setfecf(crc);
    // Move the serializer pointer to the end of the location where the trailer will be serialized
    frameSerializer.moveSerToOffset(ComCfg::TmFrameFixedSize -
                                    TMTrailer::SERIALIZED_SIZE);
    status = frameSerializer.serialize(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    this->dataOut_out(0, frameBuffer, context);
    this->dataReturnOut_out(0, data, context);  // return ownership of the original data buffer
}

void TMFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void TMFramer ::dataReturnIn_handler(FwIndexType portNum,
                                     Fw::Buffer& frameBuffer,
                                     const ComCfg::FrameContext& context) {
    // dataReturnIn is our own member buffer coming back from the dataOut call - memset it to 0
    FW_ASSERT(frameBuffer.getData() == static_cast<U8*>(this->m_frameBuffer));
    ::memset(this->m_frameBuffer, 0, sizeof(this->m_frameBuffer));
    // NOTE: should we set a flag to track that it has been returned and ready for reuse?
    // NOTE: a nice trick for efficiency would be to memset to IDLE_DATA_PATTERN instead... but eh
}

void TMFramer ::fill_with_idle_packet(Fw::SerializeBufferBase& serializer) {
    constexpr U16 endIndex = ComCfg::TmFrameFixedSize -
                           TMTrailer::SERIALIZED_SIZE;
    constexpr U16 idleApid = static_cast<U16>(ComCfg::APID::SPP_IDLE_PACKET);
    const U16 startIndex = static_cast<U16>(serializer.getBuffLength());
    const U16 idlePacketLength = endIndex - startIndex;

    FW_ASSERT(idlePacketLength > 0, static_cast<FwAssertArgType>(idlePacketLength));
    FW_ASSERT(idlePacketLength >= 7, static_cast<FwAssertArgType>(idlePacketLength)); // 7 bytes minimum for idle packet
    FW_ASSERT(idlePacketLength <= ComCfg::TmFrameFixedSize,
              static_cast<FwAssertArgType>(idlePacketLength));

    SpacePacketHeader header;
    header.setpacketIdentification(idleApid);
    header.setpacketSequenceControl(0x3 << SpacePacketSubfields::SeqFlagsOffset); // Sequence Flags = 0b11 (unsegmented) & unused Seq count
    header.setpacketDataLength(idlePacketLength);
    // Serialize header and idle data into the frame
    serializer.serialize(header);
    for (U16 i = startIndex + SpacePacketHeader::SERIALIZED_SIZE; i < endIndex; i++) {
        serializer.serialize(IDLE_DATA_PATTERN);  // Idle data
    }
}


}  // namespace CCSDS
}  // namespace Svc
