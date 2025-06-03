// ======================================================================
// \title  TCDeframer.cpp
// \author thomas-bc
// \brief  cpp file for TCDeframer component implementation class
// ======================================================================

#include "Svc/CCSDS/TCDeframer/TCDeframer.hpp"
#include "Svc/CCSDS/Types/FppConstantsAc.hpp"
#include "Svc/CCSDS/Types/TCHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/TCTrailerSerializableAc.hpp"
#include "Svc/CCSDS/Utils/CRC16.hpp"
#include "config/FpConfig.hpp"

namespace Svc {
namespace CCSDS {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

TCDeframer ::TCDeframer(const char* const compName)
    : TCDeframerComponentBase(compName),
      m_spacecraftId(ComCfg::SpacecraftId) {}

TCDeframer ::~TCDeframer() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TCDeframer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // CCSDS TC Format:
    // 5 octets - TC Primary Header
    // Up to 1019 octets - Data Field (including optional 2 octets frame error control field)

    // Note: F Prime uses Type-BD 
    // CCSDS TC Primary Header:
    // 2b - 00  - TF Version Number
    // 1b - 0/1 - Bypass Flag            (0 = Type-A FARM checks enabled, 1 = Type-B FARM checks bypassed)
    // 1b - 0/1 - Control Command Flag   (0 = Type-D data, 1 = Type-C control command)
    // 2b - 00  - Reserved Spare         (set to 00)
    // 10b- XX  - Spacecraft ID
    // 6b - XX  - Virtual Channel ID
    // 10b- XX  - Frame Length
    // 8b - XX  - Frame Sequence Number  (unused for Type-B frames)

    // CCSDS TC Trailer:
    // 16b - Frame Error Control Field (FECF): CRC16

    FW_ASSERT(data.getSize() >= TCHeader::SERIALIZED_SIZE, static_cast<FwAssertArgType>(data.getSize()));

    TCHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 frame_length = header.getvcIdAndLength() & TCSubfields::FrameLengthMask;
    U8 vc_id = static_cast<U8>((header.getvcIdAndLength() & TCSubfields::VcIdMask) >> TCSubfields::VcIdOffset);
    U16 spacecraft_id = header.getflagsAndScId() & TCSubfields::SpacecraftIdMask;

    if (spacecraft_id != this->m_spacecraftId) {
        this->log_ACTIVITY_LO_InvalidSpacecraftId(spacecraft_id, this->m_spacecraftId);
        this->dataReturnOut_out(0, data, context); // drop the frame
        return;
    }
    if (data.getSize() < static_cast<Fw::Buffer::SizeType>(frame_length + TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE)) {
        U32 maxDataAvailable = data.getSize() - TCHeader::SERIALIZED_SIZE - TCTrailer::SERIALIZED_SIZE;
        this->log_WARNING_HI_InvalidFrameLength(frame_length, maxDataAvailable);
        this->dataReturnOut_out(0, data, context); // drop the frame
        return;
    }
    if (this->m_acceptAllVcid == false && vc_id != this->m_vcId) {
        this->log_ACTIVITY_LO_InvalidVcId(vc_id, this->m_vcId);
        this->dataReturnOut_out(0, data, context); // drop the frame
        return;
    }
    // Note: F Prime uses TC Type-BD frames for now, so the FARM checks are not ran
    // This means there is no sequence count checks at the TC level (there are at the Space Packet level)

    // -------------------------------------------------
    // CRC Check
    // -------------------------------------------------
    // Compute CRC over the entire frame buffer minus the FECF trailer
    U16 computed_crc = CCSDS::Utils::CRC16::compute(data.getData(), TCHeader::SERIALIZED_SIZE + frame_length);
    TCTrailer trailer;
    auto deserializer = data.getDeserializer();
    deserializer.moveDeserToOffset(TCHeader::SERIALIZED_SIZE + frame_length);
    status = deserializer.deserialize(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 transmitted_crc = trailer.getfecf();
    if (transmitted_crc != computed_crc) {
        this->log_WARNING_HI_InvalidCrc(computed_crc, transmitted_crc);
        this->dataReturnOut_out(0, data, context); // drop the frame
        return;
    }

    // Point to the start of the data field and set appropriate size
    data.setData(data.getData() + TCHeader::SERIALIZED_SIZE);
    data.setSize(frame_length);

    this->dataOut_out(0, data, context);
}

void TCDeframer ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    this->dataReturnOut_out(0, fwBuffer, context);
}

}  // namespace CCSDS
}  // namespace Svc
