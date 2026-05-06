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
    // In its current implementation, CircularBuffer is not a LinearBufferBase, which prevents us from deserializing
    // directly from the CircularBuffer into FrameHeader/FrameTrailer. Instead, we have to copy the data into
    // a temporary SerializeBuffer, and then deserialize from that buffer into the FrameHeader/FrameTrailer objects.
    // A better implementation would be to have CircularBuffer implement a shared interface with LinearBufferBase,
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
    status = header.deserializeFrom(header_ser_buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }
    // Check that deserialized start_word token matches expected value (default start_word value in the FPP object)
    FprimeProtocol::FrameHeader default_value;
    if (header.get_startWord() != default_value.get_startWord()) {
        return Status::NO_FRAME_DETECTED;
    }
    // Validate size before proceeding.
    // Use a static_assert to guarantee the two fixed overhead constants don't themselves overflow
    // when added together. This is a compile-time check so it carries zero runtime cost, but it
    // protects the runtime guard below if SERIALIZED_SIZE values are ever changed.
    static_assert(FprimeProtocol::FrameHeader::SERIALIZED_SIZE <=
                      std::numeric_limits<FwSizeType>::max() - FprimeProtocol::FrameTrailer::SERIALIZED_SIZE,
                  "FrameHeader::SERIALIZED_SIZE + FrameTrailer::SERIALIZED_SIZE overflows FwSizeType");
    constexpr FwSizeType header_trailer_overhead =
        FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;

    // Guard: reject frames whose declared length would overflow FwSizeType when added to the
    // fixed overhead. Using subtraction on unsigned types (as in the prior implementation) is
    // fragile — if the constants change sign or width the subtraction itself can wrap silently.
    // An explicit addition-based check is clearer and easier to audit.
    if (header.get_lengthField() > std::numeric_limits<FwSizeType>::max() - header_trailer_overhead) {
        // lengthField + overhead would overflow — frame is invalid
        return Status::NO_FRAME_DETECTED;
    }

    // We expect the frame size to be size of header + body (of size specified in header) + trailer.
    // Overflow is impossible here: the guard above ensures
    //   lengthField <= MAX - header_trailer_overhead
    const FwSizeType expected_frame_size = header.get_lengthField() + header_trailer_overhead;
    // If the frame will never fit, then report NO_FRAME_DETECTED to drop the erroneous frame
    if (data.get_capacity() < expected_frame_size) {
        return Status::NO_FRAME_DETECTED;
    }
    // If the frame could fit but we haven't received enough data yet, report MORE_DATA_NEEDED
    else if (data.get_allocated_size() < expected_frame_size) {
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
    status = trailer.deserializeFrom(trailer_ser_buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return Status::NO_FRAME_DETECTED;
    }

    Utils::Hash hash;
    Utils::HashBuffer hashBuffer;
    // Compute CRC over the transmitted data (header + body).
    // Safety invariant: the guard above ensures
    //   lengthField <= MAX - header_trailer_overhead
    //                <= MAX - HEADER_SIZE - TRAILER_SIZE
    //                <  MAX - HEADER_SIZE
    // so this addition cannot overflow. The assert makes that contract explicit at the
    // point of use so it remains correct if this code is ever moved or refactored.
    FW_ASSERT(header.get_lengthField() <=
                  std::numeric_limits<FwSizeType>::max() - FprimeProtocol::FrameHeader::SERIALIZED_SIZE,
              static_cast<FwAssertArgType>(header.get_lengthField()));
    FwSizeType hash_field_size = header.get_lengthField() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE;
    hash.init();
    for (FwSizeType i = 0; i < hash_field_size; i++) {
        U8 byte = 0;
        status = data.peek(byte, i);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
        hash.update(&byte, 1);
    }
    hash.finalize(hashBuffer);

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
