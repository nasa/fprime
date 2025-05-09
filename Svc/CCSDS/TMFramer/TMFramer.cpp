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
    CCSDS::Types::TMFrameHeader header;
    U16 channelIds = 0;
    channelIds |= ComCfg::FppConstant_SpacecraftId::SpacecraftId << CCSDS::Types::TMFrameMasks::spacecraftIdOffset;
    channelIds |= ComCfg::FppConstant_VcId::VcId << CCSDS::Types::TMFrameMasks::virtualChannelIdOffset;
    channelIds |= 0x0;  // No Operational Control Field: Flag set to 0

    // Data Field Status:
    // - all flags to 0 except segment length id 0b11 per standard
    // - First Header Pointer is always 0 since we are always wrapping a single entire packet at offset 0
    U16 dataFieldStatus = 0;
    dataFieldStatus |= 0x3 << CCSDS::Types::TMFrameMasks::segLengthOffset;  // Seg Length 0b11 per Standard

    header.setchannelIds(channelIds);
    header.setmasterFrameCount(this->m_masterFrameCount);
    header.setvirtualFrameCount(this->m_virtualFrameCount);
    header.setdataFieldStatus(dataFieldStatus);

    this->m_masterFrameCount++;   // U8 intended to wrap around (modulo 256)
    this->m_virtualFrameCount++;  // U8 intended to wrap around (modulo 256)

    // -------------------------------------------------
    // Serialize the Frame
    // -------------------------------------------------

    Fw::SerializeStatus status;
    // Create frame Fw::Buffer using member data field
    Fw::Buffer frameBuffer = Fw::Buffer(this->m_frameBuffer, sizeof(this->m_frameBuffer));
    auto frameSerializer = frameBuffer.getSerializer();

    status = frameSerializer.serialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = frameSerializer.serialize(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // -------------------------------------------------
    // Trailer: Compute CRC
    // -------------------------------------------------
    CCSDS::Types::TMFrameTrailer trailer;
    // Compute CRC over the entire frame buffer minus the FECF trailer
    U16 crc = CCSDS::Utils::CRC16::compute(frameBuffer.getData(),
                                           sizeof(this->m_frameBuffer) - CCSDS::Types::TMFrameTrailer::SERIALIZED_SIZE);
    trailer.setfecf(crc);  // Frame Error Control Field
    // Move the serializer pointer to the end of the location where the trailer will be serialized
    frameSerializer.moveSerToOffset(ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize -
                                    CCSDS::Types::TMFrameTrailer::SERIALIZED_SIZE);
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
    // dataReturnIn is the member buffer coming back from the ComManager (e.g. ComStub) component
    // this->bufferDeallocate_out(0, frameBuffer);
    ::memset(this->m_frameBuffer, 0, sizeof(this->m_frameBuffer));
}

}  // namespace CCSDS

}  // namespace Svc
