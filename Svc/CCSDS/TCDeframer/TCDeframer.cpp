// ======================================================================
// \title  TCDeframer.cpp
// \author thomas-bc
// \brief  cpp file for TCDeframer component implementation class
// ======================================================================

#include "Svc/CCSDS/TCDeframer/TCDeframer.hpp"
#include "FpConfig.hpp"
#include "Svc/CCSDS/Types/FppConstantsAc.hpp"
#include "Svc/CCSDS/Types/TCFrameHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/TCFrameTrailerSerializableAc.hpp"
#include "Svc/CCSDS/Utils/CRC16.hpp"

namespace Svc {
namespace CCSDS {
// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

TCDeframer ::TCDeframer(const char* const compName) : TCDeframerComponentBase(compName) {}

TCDeframer ::~TCDeframer() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TCDeframer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // CCSDS TC Format:
    // 5 octets - TC Primary Header
    // Up to 1019 octets - Data Field (including optional 2 octets frame error control field)

    // CCSDS TC Primary Header:
    // 2b - 00  - TF Version Number
    // 1b - 0/1 - Bypass Flag            (0 = FARM checks enabled, 1 = FARM checks bypassed)
    // 1b - 0/1 - Control Command Flag   (0 = Type-D data, 1 = Type-C data)
    // 2b - 00  - Reserved Spare         (set to 00)
    // 10b- XX  - Spacecraft ID
    // 6b - XX  - Virtual Channel ID
    // 10b- XX  - Frame Length
    // 8b - XX  - Frame Sequence Number

    // CCSDS TC Trailer:
    // 16b - Frame Error Control Field (FECF): CRC16

    FW_ASSERT(data.getSize() >= CCSDS::Types::TCFrameHeader::SERIALIZED_SIZE,
              static_cast<FwAssertArgType>(data.getSize()));

    CCSDS::Types::TCFrameHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 frame_length = header.getvcIdAndLength() & CCSDS::Types::TCFrameMasks::FrameLengthMask;
    U8 vc_id =
        (header.getvcIdAndLength() & CCSDS::Types::TCFrameMasks::VcIdMask) >> CCSDS::Types::TCFrameMasks::VcIdOffset;
    U16 spacecraft_id = header.getflagsAndScId() & CCSDS::Types::TCFrameMasks::SpacecraftIdMask;
    if (spacecraft_id != ComCfg::FppConstant_SpacecraftId::SpacecraftId) {
        printf("Spacecraft ID mismatch: %d != %d\n", spacecraft_id,
               ComCfg::FppConstant_SpacecraftId::SpacecraftId);
    }
    if (data.getSize() < frame_length + CCSDS::Types::TCFrameHeader::SERIALIZED_SIZE + CCSDS::Types::TCFrameTrailer::SERIALIZED_SIZE) {
        printf("Frame length mismatch: %d != %d\n", frame_length,
               data.getSize() - CCSDS::Types::TCFrameHeader::SERIALIZED_SIZE -
                   CCSDS::Types::TCFrameTrailer::SERIALIZED_SIZE);
    }
    if (vc_id != ComCfg::FppConstant_VcId::VcId) {
        printf("VC ID mismatch: %d != %d\n", vc_id, ComCfg::FppConstant_VcId::VcId);
    }

    // -------------------------------------------------
    // CRC Check
    // -------------------------------------------------
    // Compute CRC over the entire frame buffer minus the FECF trailer
    U16 computed_crc = CCSDS::Utils::CRC16::compute(data.getData(),
                                           CCSDS::Types::TCFrameHeader::SERIALIZED_SIZE + frame_length);
    CCSDS::Types::TCFrameTrailer trailer;
    auto deserializer = data.getDeserializer();
    deserializer.moveDeserToOffset(CCSDS::Types::TCFrameHeader::SERIALIZED_SIZE + frame_length);
    status = deserializer.deserialize(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 transmitted_crc = trailer.getfecf();
    if (transmitted_crc != computed_crc) {
        printf("FECF mismatch: %d != %d\n", transmitted_crc, computed_crc);
    }

    // Point to the start of the data field and set appropriate size
    data.setData(data.getData() + Svc::CCSDS::Types::TCFrameHeader::SERIALIZED_SIZE);
    data.setSize(frame_length);

    this->dataOut_out(0, data, context);
}

void TCDeframer ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    this->dataReturnOut_out(0, fwBuffer, context);
}

}  // namespace CCSDS
}  // namespace Svc
