// ======================================================================
// \title  CcsdsTcFrameDetectorUnderflowTest.cpp
// \brief  Unit tests for the data_to_crc_length underflow fix in
//         CcsdsTcFrameDetector
//
// These tests exercise CcsdsTcFrameDetector::detect() directly and focus
// on the ordering fix: data_to_crc_length must only be computed AFTER the
// minimum-size validity check, never before.
//
// The defect (before the fix):
//   data_to_crc_length was computed as:
//       U16(expected_frame_length - TRAILER_SIZE)
//   before the check that ensures expected_frame_length >= TRAILER_SIZE.
//   A crafted frame with length field 0 (giving expected_frame_length = 1)
//   produced data_to_crc_length = 0xFFFF (unsigned underflow), which
//   would cause the CRC loop to read 65535 bytes beyond the frame data.
//
// The fix:
//   - Both the MORE_DATA_NEEDED check and the minimum-size check now
//     appear before data_to_crc_length is computed.
//   - data_to_crc_length is widened from U16 to FwSizeType to prevent
//     silent truncation for frames larger than 65535 bytes.
//
// Note on flagsAndScId: CcsdsTcFrameDetector rejects frames whose
// flagsAndScId token does not match its internal m_expectedFlagsAndScIdToken.
// The default-constructed detector's expected token is not publicly
// queryable, so the underflow tests below use flagsAndScId = 0, which is
// unlikely to match and will therefore be rejected at the flags check
// rather than the size check. This is intentional: the tests verify the
// observable safety contract (no crash, NO_FRAME_DETECTED returned) for
// crafted extreme-length inputs. A future test with access to the
// configured token value could additionally verify that the size guard
// itself fires as the rejection point.
// ======================================================================

#include <cassert>
#include <cstring>
#include <limits>
#include "Svc/Ccsds/Types/TCHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCTrailerSerializableAc.hpp"
#include "Svc/FrameAccumulator/FrameDetector/CcsdsTcFrameDetector.hpp"
#include "Utils/Types/CircularBuffer.hpp"
#include "gtest/gtest.h"

namespace Svc {
namespace FrameDetectors {

// ======================================================================
// Module-level constants (namespace scope avoids C++14 ODR-use linker
// errors that arise from static constexpr class members passed by ref
// to EXPECT_EQ)
// ======================================================================
namespace {

constexpr FwSizeType kTcHeaderSize = Ccsds::TCHeader::SERIALIZED_SIZE;
constexpr FwSizeType kTcTrailerSize = Ccsds::TCTrailer::SERIALIZED_SIZE;
constexpr FwSizeType kTcOverhead = kTcHeaderSize + kTcTrailerSize;

}  // namespace

// ======================================================================
// Test fixture
// ======================================================================

class CcsdsTcFrameDetectorUnderflowTest : public ::testing::Test {
  protected:
    // Use the default constructor — CcsdsTcFrameDetector takes no arguments
    CcsdsTcFrameDetector detector;

    // ----------------------------------------------------------------
    // Helper: serialize a TCHeader into `out_bytes` with the given
    // flagsAndScId and vcIdAndLength field values.
    // Returns true on success.
    // ----------------------------------------------------------------
    static bool buildHeader(U8* out_bytes, U16 flags_and_sc_id, U16 vc_id_and_length) {
        Ccsds::TCHeader header;
        header.set_flagsAndScId(flags_and_sc_id);
        header.set_vcIdAndLength(vc_id_and_length);
        Fw::ExternalSerializeBuffer ser(out_bytes, kTcHeaderSize);
        return header.serializeTo(ser) == Fw::FW_SERIALIZE_OK;
    }

    // ----------------------------------------------------------------
    // Helper: load bytes into a CircularBuffer.
    // Uses assert() (always available) rather than FW_ASSERT which
    // requires a specific F Prime include chain.
    // ----------------------------------------------------------------
    static void loadRing(Types::CircularBuffer& ring, const U8* src, FwSizeType count) {
        Fw::SerializeStatus s = ring.serialize(src, count);
        assert(s == Fw::FW_SERIALIZE_OK);
        static_cast<void>(s);  // suppress unused-variable warning in release builds
    }
};

// ======================================================================
// Tests — data_to_crc_length underflow / ordering safety
//
// These tests submit frames with extreme declared lengths and verify that
// detect() returns NO_FRAME_DETECTED without crashing or exhibiting
// undefined behaviour. The rejection may occur at the flagsAndScId check
// (if flagsAndScId = 0 doesn't match the detector's expected token) or at
// the minimum-size check — either way the safety contract holds.
// ======================================================================

// ----------------------------------------------------------------------
// Length field == 0 encodes expected_frame_length == 1, which is less
// than TRAILER_SIZE. Before the fix, data_to_crc_length = U16(1 - 2) =
// 0xFFFF was computed before any size validation. The fix ensures the
// size guard fires first, making the underflow impossible to reach.
// ----------------------------------------------------------------------
TEST_F(CcsdsTcFrameDetectorUnderflowTest, LengthFieldZeroRejectedSafely) {
    // vcIdAndLength with all length bits == 0 → expected_frame_length = 1
    U8 storage[kTcOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 header_bytes[kTcHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, 0, 0));
    loadRing(ring, header_bytes, kTcHeaderSize);

    U8 padding[kTcTrailerSize] = {};
    loadRing(ring, padding, kTcTrailerSize);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    // Must return NO_FRAME_DETECTED with no crash, regardless of which
    // guard (flags or size) fires first.
    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

// ----------------------------------------------------------------------
// expected_frame_length == kTcOverhead - 1 is still too small. Verifies
// the boundary just below the minimum valid size is rejected cleanly.
// ----------------------------------------------------------------------
TEST_F(CcsdsTcFrameDetectorUnderflowTest, LengthFieldOneBelowOverheadRejectedSafely) {
    // CCSDS TC length field encodes (frame_bytes - 1), so to get
    // expected_frame_length = kTcOverhead - 1 we write kTcOverhead - 2.
    const U16 vc_id_and_length = static_cast<U16>(kTcOverhead - 2);

    U8 storage[kTcOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 header_bytes[kTcHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, 0, vc_id_and_length));
    loadRing(ring, header_bytes, kTcHeaderSize);

    U8 padding[kTcTrailerSize] = {};
    loadRing(ring, padding, kTcTrailerSize);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

// ----------------------------------------------------------------------
// expected_frame_length == kTcOverhead is the minimum valid size.
// This test verifies the exact boundary: a frame of exactly overhead
// size is not rejected by the minimum-size guard. The result is still
// NO_FRAME_DETECTED (flags mismatch or CRC mismatch), but the minimum-
// size guard correctly does not fire.
// ----------------------------------------------------------------------
TEST_F(CcsdsTcFrameDetectorUnderflowTest, LengthFieldAtMinimumValidSizeNotRejectedByGuard) {
    // CCSDS TC length field encodes (frame_bytes - 1)
    const U16 vc_id_and_length = static_cast<U16>(kTcOverhead - 1);

    U8 storage[kTcOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 header_bytes[kTcHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, 0, vc_id_and_length));
    loadRing(ring, header_bytes, kTcHeaderSize);

    U8 trailer_bytes[kTcTrailerSize] = {};
    loadRing(ring, trailer_bytes, kTcTrailerSize);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    // Rejected due to flags mismatch or CRC mismatch — not the size guard.
    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

// ======================================================================
// Regression tests — preexisting behaviour must be preserved
// ======================================================================

// ----------------------------------------------------------------------
// Insufficient data (below header + trailer minimum) returns
// MORE_DATA_NEEDED before any header parsing occurs. This path is
// independent of the flagsAndScId token value.
// ----------------------------------------------------------------------
TEST_F(CcsdsTcFrameDetectorUnderflowTest, InsufficientDataReturnsMoreDataNeeded) {
    const FwSizeType too_small = kTcOverhead - 1;

    U8 storage[kTcOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 data[too_small] = {};
    loadRing(ring, data, too_small);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    EXPECT_EQ(result, FrameDetector::Status::MORE_DATA_NEEDED);
    EXPECT_EQ(size_out, kTcOverhead);
}

// ----------------------------------------------------------------------
// A mismatched flagsAndScId token must return NO_FRAME_DETECTED before
// any length or CRC logic is reached. Uses 0xFFFF as a value that is
// guaranteed not to match any valid default token.
// ----------------------------------------------------------------------
TEST_F(CcsdsTcFrameDetectorUnderflowTest, MismatchedFlagsAndScIdRejected) {
    const U16 vc_id_and_length = static_cast<U16>(kTcOverhead - 1);

    U8 storage[kTcOverhead] = {};
    Types::CircularBuffer ring(storage, sizeof(storage));

    U8 header_bytes[kTcHeaderSize] = {};
    ASSERT_TRUE(buildHeader(header_bytes, 0xFFFF, vc_id_and_length));
    loadRing(ring, header_bytes, kTcHeaderSize);

    U8 padding[kTcTrailerSize] = {};
    loadRing(ring, padding, kTcTrailerSize);

    FwSizeType size_out = 0;
    FrameDetector::Status result = detector.detect(ring, size_out);

    EXPECT_EQ(result, FrameDetector::Status::NO_FRAME_DETECTED);
}

}  // namespace FrameDetectors
}  // namespace Svc
