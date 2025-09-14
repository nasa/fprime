// ======================================================================
// \title  FprimeFrameDetector.hpp
// \author thomas-bc
// \brief  hpp file for fprime frame detector definitions
// ======================================================================

#include "Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp"

namespace Svc {
namespace FrameDetectors {

FrameDetector::Status FprimeFrameDetector::detect(const Types::CircularBuffer& data, FwSizeType& size_out) const {
    // If not enough data for header + trailer, report MORE_DATA_NEEDED
    if (data.get_allocated_size() <
        FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE) {
        size_out = FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
        return Status::MORE_DATA_NEEDED;
    }

    // NOTE: it is understood and accepted that the following code is not as efficient as it could technically be
    // We are leveraging the FPP autocoded types to do the deserialization for us.
    // In its current implementation, CircularBuffer is not a SerializeBufferBase, which prevents us from deserializing
    // directly from the CircularBuffer into FrameHeader/FrameTrailer. Instead, we have to copy the data into
    // a temporary SerializeBuffer, and then deserialize from that buffer into the FrameHeader/FrameTrailer objects.
    // A better implementation would be to have CircularBuffer implement a shared interface with SerializeBufferBase,
    // and then we could pass the CircularBuffer directly into the FrameHeader/FrameTrailer deserializers. This is left
    // as a TODO for future improvement as it is a significant refactor

    FprimeProtocol::FrameHeader header;
    FprimeProtocol::FrameTrailer trailer;

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
    if (header.get_startWord() != default_value.get_startWord()) {
        return Status::NO_FRAME_DETECTED;
    }
    // We expect the frame size to be size of header + body (of size specified in header) + trailer
    const FwSizeType expected_frame_size = FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.get_lengthField() +
                                           FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
    // If the current allocated size can't hold the expected_frame_size -> MORE_DATA_NEEDED
    if (data.get_allocated_size() < expected_frame_size) {
        size_out = expected_frame_size;
        return Status::MORE_DATA_NEEDED;
    }

    // ---------------- Frame Trailer ----------------
    U8 trailer_data[FprimeProtocol::FrameTrailer::SERIALIZED_SIZE];
    Fw::ExternalSerializeBuffer trailer_ser_buffer(trailer_data, FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    status = data.peek(trailer_data, FprimeProtocol::FrameTrailer::SERIALIZED_SIZE,
                       FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.get_lengthField());
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    status = trailer_ser_buffer.setBuffLen(FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    // Deserialize trailer from circular buffer (peeked data) into trailer object
    status = trailer.deserialize(trailer_ser_buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }

    Utils::Hash hash;
    Utils::HashBuffer hashBuffer;
    // Compute CRC over the transmitted data (header + body)
    FwSizeType hash_field_size = header.get_lengthField() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE;
    hash.init();
    for (U32 i = 0; i < hash_field_size; i++) {
        U8 byte = 0;
        status = data.peek(byte, i);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
        hash.update(&byte, 1);
    }
    hash.final(hashBuffer);

    // Compare the transmitted CRC with the computed one
    if (trailer.get_crcField() != hashBuffer.asBigEndianU32()) {
        // CRC mismatch - there likely was data corruption. The F Prime protocol
        // being very simple, we don't have a way to recover from this.
        // So we report NO_FRAME_DETECTED and drop the frame
        return Status::NO_FRAME_DETECTED;
    }
    // All checks passed - we have detected a frame of size expected_frame_size
    size_out = expected_frame_size;
    return Status::FRAME_DETECTED;
}

}  // namespace FrameDetectors
}  // namespace Svc
