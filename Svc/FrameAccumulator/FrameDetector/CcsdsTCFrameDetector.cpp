// ======================================================================
// \title  CcsdsTCFrameDetector.hpp
// \author thomas-bc
// \brief  hpp file for fprime frame detector definitions
// ======================================================================

#include "Svc/FrameAccumulator/FrameDetector/CcsdsTCFrameDetector.hpp"
#include "Svc/CCSDS/Types/FppConstantsAc.hpp"
#include <cstdio>
#include "config/FppConstantsAc.hpp"
#include "Svc/CCSDS/Types/TCFrameHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/TCFrameTrailerSerializableAc.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {
namespace FrameDetectors {

FrameDetector::Status CcsdsTCFrameDetector::detect(const Types::CircularBuffer& data, FwSizeType& size_out) const {

    if (data.get_allocated_size() < CCSDS::TCFrameHeader::SERIALIZED_SIZE) {
        size_out = CCSDS::TCFrameHeader::SERIALIZED_SIZE;
        return Status::MORE_DATA_NEEDED;
    }

    // Copy CircularBuffer data into linear buffer, for serialization into FrameHeader object
    U8 header_data[CCSDS::TCFrameHeader::SERIALIZED_SIZE];
    Fw::SerializeStatus status = data.peek(header_data, CCSDS::TCFrameHeader::SERIALIZED_SIZE, 0);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    CCSDS::TCFrameHeader header;
    Fw::ExternalSerializeBuffer header_ser_buffer(header_data, CCSDS::TCFrameHeader::SERIALIZED_SIZE);
    status = header_ser_buffer.setBuffLen(CCSDS::TCFrameHeader::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Attempt to deserialize data into the FrameHeader object
    status = header.deserialize(header_ser_buffer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 sc_id = header.getflagsAndScId() & CCSDS::TCFrameMasks::SpacecraftIdMask;
    U16 frame_length = header.getvcIdAndLength() & CCSDS::TCFrameMasks::FrameLengthMask;

    FwSizeType expected_frame_size = CCSDS::TCFrameHeader::SERIALIZED_SIZE + frame_length + CCSDS::TCFrameTrailer::SERIALIZED_SIZE; // 2 bytes for CRC
    if (sc_id == ComCfg::FppConstant_SpacecraftId::SpacecraftId) {
        size_out = expected_frame_size;
        if (data.get_allocated_size() < expected_frame_size) {
            return Status::MORE_DATA_NEEDED;
        }
        return Status::FRAME_DETECTED;
    }
    return Status::NO_FRAME_DETECTED;
}

}  // namespace FrameDetectors
}  // namespace Svc
