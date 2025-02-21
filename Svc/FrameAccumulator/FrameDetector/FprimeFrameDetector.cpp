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
    FprimeProtocol::FrameTrailer trailer;

    // If not enough data for header + trailer, report MORE_DATA_NEEDED
    if (data.get_allocated_size() < FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE) {
        size_out = FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
        return Status::MORE_DATA_NEEDED;
    }

    // NOTE: it is understood and accepted that the following code is not as efficient as it could technically be
    // We are leveraging the FPP autocoded types to do the deserialization for us.
    // In its current implementation, CircularBuffer is not a SerializeBufferBase, which prevents us from deserializing
    // directly from the CircularBuffer into FrameHeader/FrameTrailer. Instead, we have to copy the data into
    // a temporary SerializeBuffer, and then deserialize from that buffer into the FrameHeader/FrameTrailer objects.
    // A better implementation would be to have CircularBuffer implement a shared interface with SerializeBufferBase, and
    // then we could pass the CircularBuffer directly into the FrameHeader/FrameTrailer deserializers.
    // This is left as a TODO for future improvement as it is a significant refactor

    // ---------------- Frame Header ----------------
    // Copy CircularBuffer data into linear buffer, for serialization into FrameHeader object
    U8 header_data[FprimeProtocol::FrameHeader::SERIALIZED_SIZE];
    Fw::SerializeStatus status = data.peek(header_data, FprimeProtocol::FrameHeader::SERIALIZED_SIZE, 0);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    Fw::ExternalSerializeBuffer header_ser_buffer(header_data, FprimeProtocol::FrameHeader::SERIALIZED_SIZE);
    status = header_ser_buffer.setBuffLen(FprimeProtocol::FrameHeader::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Attempt to deserialize data into the FrameHeader object
    status = header.deserialize(header_ser_buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    // Check that deserialized start_word token matches expected value (default start_word value in the FPP object)
    FprimeProtocol::FrameHeader default_value;
    if (header.getstart_word() != default_value.getstart_word()) {
        return Status::NO_FRAME_DETECTED;
    }
    // If the deserialized length token can't fit in current allocated size -> MORE_DATA_NEEDED
    if (data.get_allocated_size() < FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.getlength() + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE) {
        size_out = header.getlength() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
        return Status::MORE_DATA_NEEDED;
    }

    // ---------------- Frame Trailer ----------------
    U8 trailer_data[FprimeProtocol::FrameTrailer::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer trailer_ser_buffer(trailer_data, FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    status = data.peek(trailer_data, FprimeProtocol::FrameTrailer::SERIALIZED_SIZE, FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.getlength());
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    status = trailer_ser_buffer.setBuffLen(FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Deserialize header from circular buffer (peeked data) into trailer object
    status = trailer.deserialize(trailer_ser_buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    Utils::Hash hash;
    Utils::HashBuffer hashBuffer;

    // Compute CRC over the transmitted data (header + body)
    FwSizeType hash_field_size = header.getlength() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE;
    hash.init();
    for (U32 i = 0; i < hash_field_size; i++) {
        U8 byte = 0;
        status = data.peek(byte, i);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
        hash.update(&byte, 1);
    }
    hash.final(hashBuffer);

    // Compare the transmitted CRC with the computed one
    if (trailer.getcrc() != hashBuffer.asBigEndianU32()) {
        // Note: CRC mismatch - there likely was data corruption
        // Should there be an event / telemetry for frames dropped ??
        return Status::NO_FRAME_DETECTED;
    }
    size_out = header.getlength() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
    return Status::FRAME_DETECTED;

}

}  // namespace FrameDetectors
}  // namespace Svc

