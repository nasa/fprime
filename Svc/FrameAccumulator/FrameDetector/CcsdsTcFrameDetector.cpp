// ======================================================================
// \title  CcsdsTcFrameDetector.hpp
// \author thomas-bc
// \brief  hpp file for fprime frame detector definitions
// ======================================================================

#include "Svc/FrameAccumulator/FrameDetector/CcsdsTcFrameDetector.hpp"
#include <cstdio>
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/TCHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "Utils/Hash/Hash.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {
namespace FrameDetectors {

FrameDetector::Status CcsdsTcFrameDetector::detect(const Types::CircularBuffer& data, FwSizeType& size_out) const {
    if (data.get_allocated_size() < Ccsds::TCHeader::SERIALIZED_SIZE + Ccsds::TCTrailer::SERIALIZED_SIZE) {
        size_out = Ccsds::TCHeader::SERIALIZED_SIZE + Ccsds::TCTrailer::SERIALIZED_SIZE;
        return Status::MORE_DATA_NEEDED;
    }

    // ---------------- Frame Header ----------------
    // Copy CircularBuffer data into linear buffer, for serialization into FrameHeader object
    U8 header_data[Ccsds::TCHeader::SERIALIZED_SIZE];
    Fw::SerializeStatus status = data.peek(header_data, Ccsds::TCHeader::SERIALIZED_SIZE, 0);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    Fw::ExternalSerializeBuffer header_ser_buffer(header_data, Ccsds::TCHeader::SERIALIZED_SIZE);
    status = header_ser_buffer.setBuffLen(Ccsds::TCHeader::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Attempt to deserialize data into the FrameHeader object
    Ccsds::TCHeader header;
    status = header.deserialize(header_ser_buffer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    if (header.get_flagsAndScId() != this->m_expectedFlagsAndScIdToken) {
        // If the flags and SC ID do not match the expected token, we don't have a valid frame
        return Status::NO_FRAME_DETECTED;
    }
    // TC protocol defines the Frame Length as number of bytes minus 1, so we add 1 back to get length in bytes
    const FwSizeType expected_frame_length =
        static_cast<FwSizeType>((header.get_vcIdAndLength() & Ccsds::TCSubfields::FrameLengthMask) + 1);
    const U16 data_to_crc_length = static_cast<U16>(expected_frame_length - Ccsds::TCTrailer::SERIALIZED_SIZE);

    if (data.get_allocated_size() < expected_frame_length) {
        size_out = expected_frame_length;
        return Status::MORE_DATA_NEEDED;
    }

    // ---------------- Frame Trailer ----------------
    // Compute CRC on the received data
    Ccsds::Utils::CRC16 crc;
    for (FwSizeType i = 0; i < data_to_crc_length; ++i) {
        U8 byte = 0;
        status = data.peek(byte, i);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
        crc.update(byte);
    }
    U16 computed_fecf = crc.finalize();
    // Retrieve CRC field from the trailer
    U8 trailer_data[Ccsds::TCTrailer::SERIALIZED_SIZE];
    status = data.peek(trailer_data, Ccsds::TCTrailer::SERIALIZED_SIZE, data_to_crc_length);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    Fw::ExternalSerializeBuffer trailer_ser_buffer(trailer_data, Ccsds::TCTrailer::SERIALIZED_SIZE);
    status = trailer_ser_buffer.setBuffLen(Ccsds::TCTrailer::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Attempt to deserialize data into the FrameTrailer object
    Ccsds::TCTrailer trailer;
    status = trailer.deserialize(trailer_ser_buffer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    U16 transmitted_fecf = trailer.get_fecf();
    if (transmitted_fecf != computed_fecf) {
        // If the computed CRC does not match the transmitted CRC, we don't have a valid frame
        return Status::NO_FRAME_DETECTED;
    }
    // At this point, we have validated the header and CRC - we report a valid frame detected
    size_out = expected_frame_length;
    return Status::FRAME_DETECTED;
}

}  // namespace FrameDetectors
}  // namespace Svc
