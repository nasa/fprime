// ======================================================================
// \title  FprimeFrameDetector.hpp
// \author thomas-bc
// \brief  hpp file for fprime frame detector definitions
// ======================================================================

#include "Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp"

namespace Svc {
namespace FrameDetectors {

FrameDetector::Status FprimeFrameDetector::detect(const Types::CircularBuffer& data, FwSizeType& size_out) const {

    FprimeProtocol::FrameHeader header;
    FprimeProtocol::FrameTrailer footer;

    // If not enough data for header + footer, report MORE_DATA_NEEDED
    if (data.get_allocated_size() < FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE) {
        size_out = FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
        return Status::MORE_DATA_NEEDED;
    }

    // ---------------- Frame Header ----------------
    // Copy CircularBuffer data into linear buffer, for serialization into FrameHeader object
    U8 header_data[FprimeProtocol::FrameHeader::SERIALIZED_SIZE];
    Fw::SerializeStatus status = data.peek(header_data, FprimeProtocol::FrameHeader::SERIALIZED_SIZE, 0);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    Fw::ExternalSerializeBuffer header_ser_buffer(header_data, FprimeProtocol::FrameHeader::SERIALIZED_SIZE);
    header_ser_buffer.setBuffLen(FprimeProtocol::FrameHeader::SERIALIZED_SIZE);
    // Attempt to deserialize data into the FrameHeader object
    header.deserialize(header_ser_buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    // Check that deserialized start_word token matches expected value (default start_word value in the FPP object)
    FprimeProtocol::FrameHeader default_value;
    if (header.getstart_word() != default_value.getstart_word()) {
        return Status::NO_FRAME_DETECTED;
    }
    // If the deserialized length token can not fit in current allocated size -> MORE_DATA_NEEDED
    if (data.get_allocated_size() < FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.getlength() + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE) {
        size_out = header.getlength() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
        return Status::MORE_DATA_NEEDED;
    }

    // ---------------- Frame Footer ----------------
    U8 footer_data[FprimeProtocol::FrameTrailer::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer footer_ser_buffer(footer_data, FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    status = data.peek(footer_data, FprimeProtocol::FrameTrailer::SERIALIZED_SIZE, FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.getlength());
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    footer_ser_buffer.setBuffLen(FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    // Desiralize header from circular buffer (peeked data) into footer object
    footer.deserialize(footer_ser_buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    Utils::Hash hash;
    Utils::HashBuffer hashBuffer;

    // Compute CRC over the transmitted data (header + body)
    hash.init();
    for (U32 i = 0; i < FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.getlength(); i++) {
        U8 byte = 0;
        status = data.peek(byte, i);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
        hash.update(&byte, 1);
    }
    hash.final(hashBuffer);

    // printf("transmitted_crc: %08X\n", footer.getcrc());
    // printf("computed_crc: %08X\n", hashBuffer.asBigEndianU32());
    // Compare the transmitted CRC with the computed one
    if (footer.getcrc() != hashBuffer.asBigEndianU32()) {
        // Note: CRC mismatch - there likely was data corruption
        // Should there be an event / telemetry for frames dropped ??
        FW_ASSERT(footer.getcrc() == hashBuffer.asBigEndianU32());
        return Status::NO_FRAME_DETECTED;
    }
    size_out = header.getlength() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
    return Status::FRAME_DETECTED;

    
    // Issues:
    // 1. Can't retrieve the type info directly? Have to make something of size SERIALIZED_SIZE ?
    //        Not a huge deal, but not as efficient ?
    // 2. Can't use the serialization magic since CircularBuffer is not a SierializeBufferBase ???


}

};  // namespace FrameDetectors
};  // namespace Svc

