// ======================================================================
// \title  TMFramer.cpp
// \author thomas-bc
// \brief  cpp file for TMFramer component implementation class
// ======================================================================

#include "Svc/CCSDS/TMFramer/TMFramer.hpp"
#include "Svc/CCSDS/Types/FppConstantsAc.hpp"
#include "Svc/CCSDS/Types/TMFrameHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/TMFrameTrailerSerializableAc.hpp"
#include "Svc/CCSDS/Utils/CRC16.hpp"
#include "config/FppConstantsAc.hpp"

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
    FW_ASSERT(data.getSize() <= ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize,
              static_cast<FwAssertArgType>(data.getSize()));

    // -----------------------------------------------
    // Header
    // -----------------------------------------------
    TMFrameHeader header;

    U16 globalVcId = 0;
    globalVcId |= ComCfg::FppConstant_SpacecraftId::SpacecraftId << TMFrameMasks::spacecraftIdOffset;
    globalVcId |= context.getvcId() << TMFrameMasks::virtualChannelIdOffset;
    globalVcId |= 0x0;  // Operational Control Field: Flag set to 0

    // Data Field Status:
    // - all flags to 0 except segment length id 0b11 per standard
    // - First Header Pointer is always 0 since we are always wrapping a single entire packet at offset 0
    U16 dataFieldStatus = 0;
    dataFieldStatus |= 0x3 << TMFrameMasks::segLengthOffset;  // Seg Length 0b11 per Standard

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

    Fw::SerializeStatus status;
    // Create frame Fw::Buffer using member data field
    Fw::Buffer frameBuffer = Fw::Buffer(this->m_frameBuffer, sizeof(this->m_frameBuffer));
    auto frameSerializer = frameBuffer.getSerializer();

    status = frameSerializer.serialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = frameSerializer.serialize(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    this->fill_with_idle_packet(static_cast<U16>(frameSerializer.getBuffLength()));

    // -------------------------------------------------
    // Trailer (CRC)
    // -------------------------------------------------
    TMFrameTrailer trailer;
    // Compute CRC over the entire frame buffer minus the FECF trailer
    U16 crc = CCSDS::Utils::CRC16::compute(frameBuffer.getData(),
                                           sizeof(this->m_frameBuffer) - TMFrameTrailer::SERIALIZED_SIZE);
    // Set the Frame Error Control Field (FECF)
    trailer.setfecf(crc);
    // Move the serializer pointer to the end of the location where the trailer will be serialized
    frameSerializer.moveSerToOffset(ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize -
                                    TMFrameTrailer::SERIALIZED_SIZE);
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
    ::memset(this->m_frameBuffer, 0, sizeof(this->m_frameBuffer));
    // NOTE: should I set a flag to track that it has been returned an ready for reuse?
}

void TMFramer ::fill_with_idle_packet(U16 startIndex) {
    // TODO: make this code cleaner - could request from SpacePacket ??
    U16 endIndex = ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize -
                           TMFrameTrailer::SERIALIZED_SIZE;
    U16 idlePacketLength = endIndex - startIndex;
    FW_ASSERT(idlePacketLength > 0, static_cast<FwAssertArgType>(idlePacketLength));
    FW_ASSERT(idlePacketLength >= 7, static_cast<FwAssertArgType>(idlePacketLength)); // 7 bytes minimum for idle packet
    FW_ASSERT(idlePacketLength <= ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize,
              static_cast<FwAssertArgType>(idlePacketLength));
    U16 idleApid = 0x7FF;     // All 1s (11bit) per Space Packet protocol paragraph 4.1.3.3.4.4
    this->m_frameBuffer[startIndex + 0] = (idleApid >> 8) & 0xFF;
    this->m_frameBuffer[startIndex + 1] = idleApid & 0xFF;
    this->m_frameBuffer[startIndex + 2] = 0xC0;  // Sequence Flags = 0b11 (unsegmented) & unused Seq count
    this->m_frameBuffer[startIndex + 3] = 0x00;  // unused Sequence Count 
    this->m_frameBuffer[startIndex + 4] = idlePacketLength >> 8;    // Packet Data Length MSB
    this->m_frameBuffer[startIndex + 5] = idlePacketLength & 0xFF;  // Packet Data Length LSB
    // Fill the rest of the buffer with arbitrary idle data
    for (U16 i = startIndex + 6; i < endIndex; i++) {
        this->m_frameBuffer[i] = IDLE_DATA_PATTERN;  // Idle data
    }
}


}  // namespace CCSDS
}  // namespace Svc
