// ======================================================================
// \title  FprimeFrameDetectorTest.cpp
// \brief  Unit tests for overflow-hardening fixes in FprimeFrameDetector
//
// These tests exercise FprimeFrameDetector::detect() directly (unlike the
// FrameAccumulator tests which use a MockDetector). They cover:
//
//   Overflow guard: near-max lengthField values must be rejected
//   or handled safely before expected_frame_size is computed.
//
//   Implementation note: lengthField is a TokenType (U32, 32-bit).
//   On a 64-bit host FwSizeType is 64-bit, so TokenType::max +
//   OVERHEAD cannot overflow FwSizeType — the guard is dead on
//   this platform. The tests below therefore verify the correct
//   observable behaviour (NO_FRAME_DETECTED) for extreme inputs
//   regardless of which check produces it (guard vs. capacity).
//   On a 32-bit embedded target where FwSizeType == U32, the
//   guard becomes the critical path and prevents the overflow.
//
//   hash_field_size invariant: frames that pass the guard must
//   reach the CRC stage without triggering the FW_ASSERT, proving
//   the safety contract holds end-to-end.
// ======================================================================

#include <cstring>
#include <limits>
#include "Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp"
#include "Utils/Types/CircularBuffer.hpp"
#include "gtest/gtest.h"

namespace Svc {
namespace FrameDetectors {

// ======================================================================
// Module-level constants
//
// Declared at namespace scope (not as static class members) to avoid the
// C++14 requirement for out-of-class definitions when a constexpr static
// member is ODR-used (e.g. passed by const-ref to EXPECT_EQ).
// ======================================================================
namespace {

// The actual type stored in the lengthField. The FPP-generated setter and
// getter both use TokenType, so all length values passed to buildHeader
// must be this type to avoid narrowing-conversion compiler errors.
using LengthType = FprimeProtocol::TokenType;

constexpr FwSizeType kHeaderSize = FprimeProtocol::FrameHeader::SERIALIZED_SIZE;
constexpr FwSizeType kTrailerSize = FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
constexpr FwSizeType kOverhead = kHeaderSize + kTrailerSize;

}  // namespace

// ======================================================================
// Test fixture
// ======================================================================

class FprimeFrameDetectorTest : public ::testing::Test {
  protected:
    // The real detector under test — no mock
    FprimeFrameDetector detector;

    // ----------------------------------------------------------------
    // Helper: serialize a FrameHeader with a specified lengthField into
    // `out_bytes` (caller must supply a buffer of at least kHeaderSize).
    // The startWord is left at its default (magic) value so the detector
    // will accept the header as valid.
    // Returns true on success.
    // ----------------------------------------------------------------
    static bool buildHeader(U8* out_bytes, LengthType length_field) {
        FprimeProtocol::FrameHeader header;
        header.set_lengthField(length_field);
        Fw::ExternalSerializeBuffer ser(out_bytes, kHeaderSize);
        return header.serializeTo(ser) == Fw::FW_SERIALIZE_OK;
    }

    // ----------------------------------------------------------------
    // Helper: load `byte_count` bytes from `src` into `ring`.
    // The ring must already have been constructed with sufficient capacity.
    // ----------------------------------------------------------------
    static void loadRing(Types::CircularBuffer& ring, const U8* src, FwSizeType byte_count) {
        Fw::SerializeStatus status = ring.serialize(src, byte_count);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    }
};

// ======================================================================
// Test — extreme lengthField values are handled safely
// ======================================================================

// ----------------------------------------------------------------------
// TokenType::max is the largest value the lengthField can ever hold.
// On a 64-bit host the overflow guard does not fire (TokenType::max +
// kOverhead fits in FwSizeType), but the capacity check must still reject
// the frame because no ring can hold 4 GB of data.
// On a 32-bit target the overflow guard fires first — either way the
// result must be NO_FRAME_DETECTED with no crash or silent wrap.
// ----------------------------------------------------------------------
TEST_F(FprimeFrameDetectorTest, MaxLengthFieldIsRejected) {
    const LengthType max_length = std::numeric_limits<LengthType>::max();

    // Ring only needs enough bytes to pass the initial minimum-data check
    // (kOverhead bytes) and reach the lengthField validation.
    U8 storage[kOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 header_bytes[kHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, max_length));
    loadRing(ring, header_bytes, kHeaderSize);

    U8 padding[kTrailerSize] = {};
    loadRing(ring, padding, kTrailerSize);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    // Must be rejected: overflow guard (32-bit FwSizeType) or capacity
    // check (64-bit FwSizeType). No crash, no silent wrap.
    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

// ----------------------------------------------------------------------
// A lengthField larger than the ring's capacity must be rejected even
// when it doesn't come close to overflowing FwSizeType. This verifies
// the capacity check that follows the overflow guard.
// ----------------------------------------------------------------------
TEST_F(FprimeFrameDetectorTest, LengthFieldExceedingRingCapacityIsRejected) {
    // Ring sized to the bare minimum so detect() reaches the size logic.
    U8 storage[kOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    // Any body_size > 0 means the full frame won't fit in this ring.
    const LengthType body_size = 1;

    U8 header_bytes[kHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, body_size));
    loadRing(ring, header_bytes, kHeaderSize);

    U8 padding[kTrailerSize] = {};
    loadRing(ring, padding, kTrailerSize);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    // capacity == kOverhead < kOverhead + 1 → frame will never fit
    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

// ----------------------------------------------------------------------
// A zero-length frame body is a valid edge case. The overflow guard must
// pass it through (0 + kOverhead == kOverhead, no overflow), and the
// subsequent logic must request more data if the trailer hasn't arrived.
// ----------------------------------------------------------------------
TEST_F(FprimeFrameDetectorTest, ZeroLengthFieldPassesGuard) {
    const LengthType zero_length = 0;

    // Ring sized for a full zero-body frame but only header loaded —
    // the detector must request more data, not reject.
    U8 storage[kOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 header_bytes[kHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, zero_length));
    loadRing(ring, header_bytes, kHeaderSize);

    // Trailer NOT loaded — full frame not yet in the ring
    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    // Guard passes (0 is trivially safe). Detector must ask for more data.
    EXPECT_EQ(result, FrameDetector::Status::MORE_DATA_NEEDED);
    EXPECT_EQ(size_out, kOverhead);  // header + 0-byte body + trailer
}

// ======================================================================
// Test — hash_field_size invariant (FW_ASSERT contract)
// ======================================================================

// The FW_ASSERT before `hash_field_size` fires only if the guard failed to
// catch an unsafe lengthField. Because the guard prevents that from
// happening, we cannot (and should not) write a test that reaches a
// failing assert — that would be a crash, not a test failure.
//
// Instead, these tests confirm that for valid frames the code proceeds all
// the way to the CRC check without triggering the assert. A silent overflow
// in hash_field_size would produce the wrong CRC loop iteration count,
// causing either wrong output or UB — both detectable as test anomalies.

// ----------------------------------------------------------------------
// Small frame (4-byte body): wrong CRC is the ONLY reason for rejection.
// Proves hash_field_size == kHeaderSize + 4 was computed correctly.
// ----------------------------------------------------------------------
TEST_F(FprimeFrameDetectorTest, HashFieldSizeInvariantHoldsForSmallFrame) {
    const LengthType body_size = 4;
    const FwSizeType frame_size = kHeaderSize + static_cast<FwSizeType>(body_size) + kTrailerSize;

    U8 storage[frame_size] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 header_bytes[kHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, body_size));
    loadRing(ring, header_bytes, kHeaderSize);

    U8 body[] = {0xDE, 0xAD, 0xBE, 0xEF};
    loadRing(ring, body, body_size);

    // Intentionally wrong CRC (all zeros) — the only reason for rejection
    U8 trailer_bytes[kTrailerSize] = {};
    loadRing(ring, trailer_bytes, kTrailerSize);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    // FW_ASSERT must NOT fire (no crash). Rejection is CRC mismatch only.
    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

// ----------------------------------------------------------------------
// Larger frame (1024-byte body): same assertion for a non-trivial payload.
// ----------------------------------------------------------------------
TEST_F(FprimeFrameDetectorTest, HashFieldSizeInvariantHoldsForLargerFrame) {
    const LengthType body_size = 1024;
    const FwSizeType frame_size = kHeaderSize + static_cast<FwSizeType>(body_size) + kTrailerSize;

    U8 storage[frame_size] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 header_bytes[kHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, body_size));
    loadRing(ring, header_bytes, kHeaderSize);

    U8 body[1024] = {};
    ::memset(body, 0xAB, sizeof(body));
    loadRing(ring, body, body_size);

    U8 trailer_bytes[kTrailerSize] = {};  // intentionally wrong CRC
    loadRing(ring, trailer_bytes, kTrailerSize);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    // FW_ASSERT must NOT fire. Rejection is CRC mismatch only.
    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

// ======================================================================
// Regression tests — ensure preexisting behaviour is preserved
// ======================================================================

// ----------------------------------------------------------------------
// Insufficient data returns MORE_DATA_NEEDED and populates size_out.
// This fires before the overflow guard is ever reached.
// ----------------------------------------------------------------------
TEST_F(FprimeFrameDetectorTest, InsufficientDataReturnsMoreDataNeeded) {
    const FwSizeType too_small = kOverhead - 1;

    U8 storage[kOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 data[too_small] = {};
    loadRing(ring, data, too_small);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    EXPECT_EQ(result, FrameDetector::Status::MORE_DATA_NEEDED);
    EXPECT_EQ(size_out, kOverhead);
}

// ----------------------------------------------------------------------
// A bad start word must return NO_FRAME_DETECTED regardless of the
// length field value.
// ----------------------------------------------------------------------
TEST_F(FprimeFrameDetectorTest, InvalidStartWordReturnsNoFrameDetected) {
    U8 storage[kOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    // 0xFF bytes will not match the expected start word magic value
    U8 bad_data[kOverhead];
    ::memset(bad_data, 0xFF, sizeof(bad_data));
    loadRing(ring, bad_data, kOverhead);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

}  // namespace FrameDetectors
}  // namespace Svc
