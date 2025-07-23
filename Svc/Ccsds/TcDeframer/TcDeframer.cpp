// ======================================================================
// \title  TcDeframer.cpp
// \author thomas-bc
// \brief  cpp file for TcDeframer component implementation class
// ======================================================================

#include "Svc/Ccsds/TcDeframer/TcDeframer.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/TCHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "config/FpConfig.hpp"

namespace Svc {
namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

TcDeframer ::TcDeframer(const char* const compName)
    : TcDeframerComponentBase(compName), m_spacecraftId(ComCfg::SpacecraftId) {}

TcDeframer ::~TcDeframer() {}

void TcDeframer::configure(U16 vcId, U16 spacecraftId, bool acceptAllVcid) {
    this->m_vcId = vcId;
    this->m_spacecraftId = spacecraftId;
    this->m_acceptAllVcid = acceptAllVcid;
}
// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TcDeframer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
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

    FW_ASSERT(data.getSize() > TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE,
              static_cast<FwAssertArgType>(data.getSize()));

    TCHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // TC protocol defines the Frame Length as number of bytes minus 1, so we add 1 back to get length in bytes
    U16 total_frame_length = static_cast<U16>((header.get_vcIdAndLength() & TCSubfields::FrameLengthMask) + 1);
    U8 vc_id = static_cast<U8>((header.get_vcIdAndLength() & TCSubfields::VcIdMask) >> TCSubfields::VcIdOffset);
    U16 spacecraft_id = header.get_flagsAndScId() & TCSubfields::SpacecraftIdMask;

    if (spacecraft_id != this->m_spacecraftId) {
        this->log_WARNING_LO_InvalidSpacecraftId(spacecraft_id, this->m_spacecraftId);
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }
    if (data.getSize() < static_cast<Fw::Buffer::SizeType>(total_frame_length)) {
        FwSizeType maxDataAvailable = static_cast<FwSizeType>(data.getSize());
        this->log_WARNING_HI_InvalidFrameLength(total_frame_length, maxDataAvailable);
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }
    if (not this->m_acceptAllVcid && vc_id != this->m_vcId) {
        this->log_ACTIVITY_LO_InvalidVcId(vc_id, this->m_vcId);
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }
    // Note: F Prime uses TC Type-BD frames for now, so the FARM checks are not ran
    // This means there is no sequence count checks at the TC level (there are at the Space Packet level)

    // -------------------------------------------------
    // CRC Check
    // -------------------------------------------------
    // Compute CRC over the entire frame buffer minus the FECF trailer
    U16 computed_crc = Ccsds::Utils::CRC16::compute(data.getData(), total_frame_length - TCTrailer::SERIALIZED_SIZE);
    TCTrailer trailer;
    auto deserializer = data.getDeserializer();
    deserializer.moveDeserToOffset(total_frame_length - TCTrailer::SERIALIZED_SIZE);
    status = deserializer.deserialize(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 transmitted_crc = trailer.get_fecf();
    if (transmitted_crc != computed_crc) {
        this->log_WARNING_HI_InvalidCrc(computed_crc, transmitted_crc);
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }

    // Point to the start of the data field and set appropriate size
    data.setData(data.getData() + TCHeader::SERIALIZED_SIZE);
    // Shrink size to that of the encapsulated data field ( header | data | trailer )
    data.setSize(total_frame_length - TCHeader::SERIALIZED_SIZE - TCTrailer::SERIALIZED_SIZE);

    this->dataOut_out(0, data, context);
}

void TcDeframer ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    this->dataReturnOut_out(0, fwBuffer, context);
}

}  // namespace Ccsds
}  // namespace Svc
