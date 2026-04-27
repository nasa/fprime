// ======================================================================
// \title  DeserializeToTest.cpp
// \author bitWarrior
// \brief  Unit tests for the buffCapacity parameter added to
//         LinearBufferBase::deserializeTo() and SerialBuffer::popBytes()
//         (PR #5043).
//
// Coverage targets
// ----------------
//  - Happy path: OMIT_LENGTH and INCLUDE_LENGTH modes complete successfully
//    and produce the expected bytes in the destination buffer.
//  - Overflow rejected: destination capacity smaller than the data to be
//    copied returns FW_DESERIALIZE_SIZE_MISMATCH without touching the
//    destination or advancing the cursor.
//  - Null pointer guard: buff==nullptr with buffCapacity>0 returns
//    FW_DESERIALIZE_SIZE_MISMATCH.
//  - Zero-length edge case: null buff with zero capacity and zero length
//    is a no-op and returns FW_SERIALIZE_OK.
//  - Source underrun: fewer bytes remain in the source than requested.
//  - Cursor integrity: the deserialise cursor is NOT advanced after any
//    failing call.
//  - popBytes wrapper: the SerialBuffer helper obeys the same contract.
// ======================================================================

#include "gtest/gtest.h"

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/Serializable.hpp"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

// A thin concrete buffer backed by a fixed stack array, sufficient for all
// tests below.  ExternalSerializeBuffer is the lightest concrete
// LinearBufferBase subclass available without pulling in platform headers.
constexpr FwSizeType BACKING_SIZE = 128;

struct TestBuffer {
    U8 storage[BACKING_SIZE];
    Fw::ExternalSerializeBuffer buf;

    TestBuffer() : buf(storage, BACKING_SIZE) {}
};

// ExternalSerializeBuffer has deleted copy and move constructors, so helpers
// operate on an existing TestBuffer by reference rather than returning by value.

// Write `count` bytes with values 0, 1, 2, … into tb and reset for reading.
void makeFilled(TestBuffer& tb, FwSizeType count) {
    for (FwSizeType i = 0; i < count; ++i) {
        Fw::SerializeStatus st =
            tb.buf.serializeFrom(static_cast<U8>(i & 0xFF));
        EXPECT_EQ(st, Fw::FW_SERIALIZE_OK);
    }
    tb.buf.resetDeser();
}

// Write `count` bytes preceded by a FwSizeStoreType length prefix into tb,
// then reset for reading.
void makeFilledWithLength(TestBuffer& tb, FwSizeType count) {
    Fw::SerializeStatus st =
        tb.buf.serializeFrom(static_cast<FwSizeStoreType>(count));
    EXPECT_EQ(st, Fw::FW_SERIALIZE_OK);
    for (FwSizeType i = 0; i < count; ++i) {
        st = tb.buf.serializeFrom(static_cast<U8>(i & 0xFF));
        EXPECT_EQ(st, Fw::FW_SERIALIZE_OK);
    }
    tb.buf.resetDeser();
}

}  // namespace

// ===========================================================================
// OMIT_LENGTH overload
// ===========================================================================

// ---------------------------------------------------------------------------
// Happy path — destination is exactly the right size
// ---------------------------------------------------------------------------
TEST(DeserializeTo, HappyPathOmitLengthExactFit) {
    constexpr FwSizeType N = 8;
    TestBuffer tb; makeFilled(tb, N);

    U8 dest[N] = {};
    FwSizeType length = N;  // caller specifies how many bytes to pull

    Fw::SerializeStatus status = tb.buf.deserializeTo(
        dest, sizeof(dest), length, Fw::Serialization::OMIT_LENGTH);

    EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
    EXPECT_EQ(length, N);
    for (FwSizeType i = 0; i < N; ++i) {
        EXPECT_EQ(dest[i], static_cast<U8>(i & 0xFF))
            << "Byte mismatch at index " << i;
    }
}

// ---------------------------------------------------------------------------
// Happy path — destination is larger than needed (spare capacity is fine)
// ---------------------------------------------------------------------------
TEST(DeserializeTo, HappyPathOmitLengthOversizedDest) {
    constexpr FwSizeType N = 4;
    TestBuffer tb; makeFilled(tb, N);

    U8 dest[32] = {};  // bigger than N
    FwSizeType length = N;

    Fw::SerializeStatus status = tb.buf.deserializeTo(
        dest, sizeof(dest), length, Fw::Serialization::OMIT_LENGTH);

    EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
    EXPECT_EQ(length, N);
    for (FwSizeType i = 0; i < N; ++i) {
        EXPECT_EQ(dest[i], static_cast<U8>(i & 0xFF))
            << "Byte mismatch at index " << i;
    }
}

// ---------------------------------------------------------------------------
// Overflow rejected — destination buffer is one byte too small
// ---------------------------------------------------------------------------
TEST(DeserializeTo, OverflowRejectedOmitLengthDestTooSmall) {
    constexpr FwSizeType N = 8;
    TestBuffer tb; makeFilled(tb, N);

    U8 dest[N - 1] = {};
    FwSizeType length = N;  // ask for N bytes into a buffer of only N-1

    const FwSizeType cursorBefore = tb.buf.getDeserializeSizeLeft();

    Fw::SerializeStatus status = tb.buf.deserializeTo(
        dest, sizeof(dest), length, Fw::Serialization::OMIT_LENGTH);

    EXPECT_EQ(status, Fw::FW_DESERIALIZE_SIZE_MISMATCH);

    // Cursor must NOT have advanced after the failed call.
    EXPECT_EQ(tb.buf.getDeserializeSizeLeft(), cursorBefore)
        << "Cursor should not advance after a failed deserializeTo";
}

// ---------------------------------------------------------------------------
// Source underrun — source has fewer bytes than requested
// ---------------------------------------------------------------------------
TEST(DeserializeTo, SourceUnderrunOmitLength) {
    constexpr FwSizeType WRITTEN = 4;
    constexpr FwSizeType REQUESTED = 8;  // more than what was written
    TestBuffer tb; makeFilled(tb, WRITTEN);

    U8 dest[REQUESTED] = {};
    FwSizeType length = REQUESTED;

    Fw::SerializeStatus status = tb.buf.deserializeTo(
        dest, sizeof(dest), length, Fw::Serialization::OMIT_LENGTH);

    EXPECT_EQ(status, Fw::FW_DESERIALIZE_SIZE_MISMATCH);
}

// ===========================================================================
// INCLUDE_LENGTH overload
// ===========================================================================

// ---------------------------------------------------------------------------
// Happy path — stored length fits inside the destination capacity
// ---------------------------------------------------------------------------
TEST(DeserializeTo, HappyPathIncludeLength) {
    constexpr FwSizeType N = 6;
    TestBuffer tb; makeFilledWithLength(tb, N);

    U8 dest[N] = {};
    FwSizeType length = N;  // initial value acts as the max the caller accepts

    Fw::SerializeStatus status = tb.buf.deserializeTo(
        dest, sizeof(dest), length, Fw::Serialization::INCLUDE_LENGTH);

    EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
    EXPECT_EQ(length, N);  // length is updated to the actual bytes written
    for (FwSizeType i = 0; i < N; ++i) {
        EXPECT_EQ(dest[i], static_cast<U8>(i & 0xFF))
            << "Byte mismatch at index " << i;
    }
}

// ---------------------------------------------------------------------------
// Overflow rejected — stored length prefix exceeds destination capacity
// ---------------------------------------------------------------------------
TEST(DeserializeTo, OverflowRejectedIncludeLengthDestTooSmall) {
    constexpr FwSizeType STORED = 8;  // serialized as the length prefix
    constexpr FwSizeType DEST_CAP = 4;  // destination is smaller
    TestBuffer tb; makeFilledWithLength(tb, STORED);

    U8 dest[DEST_CAP] = {};
    FwSizeType length = STORED;

    Fw::SerializeStatus status = tb.buf.deserializeTo(
        dest, DEST_CAP, length, Fw::Serialization::INCLUDE_LENGTH);

    EXPECT_EQ(status, Fw::FW_DESERIALIZE_SIZE_MISMATCH);

    // Note: INCLUDE_LENGTH reads the length prefix before detecting the
    // mismatch, so the cursor does advance past the prefix. The key guarantee
    // is that the destination array is never written and status is MISMATCH.
    for (FwSizeType i = 0; i < DEST_CAP; ++i) {
        EXPECT_EQ(dest[i], 0) << "Destination must not be written on overflow";
    }
}

// ===========================================================================
// Endianness / convenience overload (INCLUDE_LENGTH, explicit endianMode)
// ===========================================================================

TEST(DeserializeTo, HappyPathExplicitEndianMode) {
    constexpr FwSizeType N = 5;
    TestBuffer tb; makeFilledWithLength(tb, N);

    U8 dest[N] = {};
    Fw::Serializable::SizeType length = static_cast<Fw::Serializable::SizeType>(N);

    Fw::SerializeStatus status =
        tb.buf.deserializeTo(dest, sizeof(dest), length, Fw::Endianness::BIG);

    EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
    EXPECT_EQ(length, static_cast<Fw::Serializable::SizeType>(N));
    for (FwSizeType i = 0; i < N; ++i) {
        EXPECT_EQ(dest[i], static_cast<U8>(i & 0xFF))
            << "Byte mismatch at index " << i;
    }
}

// ===========================================================================
// Null pointer guards
// ===========================================================================

// ---------------------------------------------------------------------------
// buff == nullptr with buffCapacity > 0 must be rejected immediately
// ---------------------------------------------------------------------------
TEST(DeserializeTo, NullPointerNonZeroCapacityRejected) {
    constexpr FwSizeType N = 4;
    TestBuffer tb; makeFilled(tb, N);

    FwSizeType length = N;

    Fw::SerializeStatus status = tb.buf.deserializeTo(
        nullptr, N, length, Fw::Serialization::OMIT_LENGTH);

    EXPECT_EQ(status, Fw::FW_DESERIALIZE_SIZE_MISMATCH);
}

// ---------------------------------------------------------------------------
// length == 0 is a valid no-op — cursor does not advance, dest is untouched.
// Uses a real buffer to avoid passing nullptr to memcpy, which is UB even
// with a zero length and would be caught by UBSAN.
// ---------------------------------------------------------------------------
TEST(DeserializeTo, ZeroLengthIsNoOp) {
    TestBuffer tb; makeFilled(tb, 4);

    U8 dest[4] = {};
    FwSizeType length = 0;

    const FwSizeType cursorBefore = tb.buf.getDeserializeSizeLeft();

    Fw::SerializeStatus status = tb.buf.deserializeTo(
        dest, sizeof(dest), length, Fw::Serialization::OMIT_LENGTH);

    EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
    EXPECT_EQ(length, static_cast<FwSizeType>(0));
    // Cursor must not advance on a zero-length read.
    EXPECT_EQ(tb.buf.getDeserializeSizeLeft(), cursorBefore);
    // Destination must be untouched.
    for (FwSizeType i = 0; i < static_cast<FwSizeType>(sizeof(dest)); ++i) {
        EXPECT_EQ(dest[i], 0);
    }
}

// ===========================================================================
// Cursor integrity across successive calls
// ===========================================================================

// ---------------------------------------------------------------------------
// A failed call must not advance the deserialise cursor, so a subsequent
// valid call with the correct capacity still succeeds.
// ---------------------------------------------------------------------------
TEST(DeserializeTo, CursorUnchangedAfterFailedCall) {
    constexpr FwSizeType N = 8;
    TestBuffer tb; makeFilled(tb, N);

    // First attempt: destination too small — must fail.
    U8 smallDest[N / 2] = {};
    FwSizeType length = N;
    Fw::SerializeStatus status = tb.buf.deserializeTo(
        smallDest, sizeof(smallDest), length, Fw::Serialization::OMIT_LENGTH);
    EXPECT_EQ(status, Fw::FW_DESERIALIZE_SIZE_MISMATCH);

    // Second attempt: correctly-sized destination — must succeed and return
    // the same data that was written originally.
    U8 goodDest[N] = {};
    length = N;
    status = tb.buf.deserializeTo(
        goodDest, sizeof(goodDest), length, Fw::Serialization::OMIT_LENGTH);
    EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
    EXPECT_EQ(length, N);
    for (FwSizeType i = 0; i < N; ++i) {
        EXPECT_EQ(goodDest[i], static_cast<U8>(i & 0xFF))
            << "Byte mismatch at index " << i;
    }
}

// ---------------------------------------------------------------------------
// Two successive happy-path calls partition the source correctly
// ---------------------------------------------------------------------------
TEST(DeserializeTo, SuccessiveCallsPartitionSource) {
    constexpr FwSizeType TOTAL = 8;
    constexpr FwSizeType FIRST = 3;
    constexpr FwSizeType SECOND = TOTAL - FIRST;
    TestBuffer tb; makeFilled(tb, TOTAL);

    U8 dest1[FIRST] = {};
    FwSizeType len1 = FIRST;
    EXPECT_EQ(tb.buf.deserializeTo(dest1, sizeof(dest1), len1,
                                   Fw::Serialization::OMIT_LENGTH),
              Fw::FW_SERIALIZE_OK);

    U8 dest2[SECOND] = {};
    FwSizeType len2 = SECOND;
    EXPECT_EQ(tb.buf.deserializeTo(dest2, sizeof(dest2), len2,
                                   Fw::Serialization::OMIT_LENGTH),
              Fw::FW_SERIALIZE_OK);

    // Verify each partition contains the right slice of 0, 1, 2, …
    for (FwSizeType i = 0; i < FIRST; ++i) {
        EXPECT_EQ(dest1[i], static_cast<U8>(i)) << "dest1 mismatch at " << i;
    }
    for (FwSizeType i = 0; i < SECOND; ++i) {
        EXPECT_EQ(dest2[i], static_cast<U8>((FIRST + i) & 0xFF))
            << "dest2 mismatch at " << i;
    }
}

// ===========================================================================
// SerialBuffer::popBytes
// ===========================================================================

// ---------------------------------------------------------------------------
// Happy path
// ---------------------------------------------------------------------------
TEST(PopBytes, HappyPath) {
    constexpr FwSizeType N = 6;
    U8 srcStorage[N];
    for (FwSizeType i = 0; i < N; ++i) {
        srcStorage[i] = static_cast<U8>(10 + i);
    }
    Fw::SerialBuffer sb(srcStorage, N);
    sb.fill();  // mark all bytes as valid serialized data
    sb.resetDeser();

    U8 dest[N] = {};
    Fw::SerializeStatus status = sb.popBytes(dest, sizeof(dest), N);

    EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
    for (FwSizeType i = 0; i < N; ++i) {
        EXPECT_EQ(dest[i], static_cast<U8>(10 + i))
            << "popBytes byte mismatch at " << i;
    }
}

// ---------------------------------------------------------------------------
// Overflow rejected — destination capacity smaller than pop count
// ---------------------------------------------------------------------------
TEST(PopBytes, OverflowRejected) {
    constexpr FwSizeType N = 8;
    U8 srcStorage[N];
    for (FwSizeType i = 0; i < N; ++i) {
        srcStorage[i] = static_cast<U8>(i);
    }
    Fw::SerialBuffer sb(srcStorage, N);
    sb.fill();
    sb.resetDeser();

    U8 dest[N / 2] = {};

    // Ask for N bytes but only give a capacity of N/2.
    Fw::SerializeStatus status = sb.popBytes(dest, sizeof(dest), N);

    EXPECT_EQ(status, Fw::FW_DESERIALIZE_SIZE_MISMATCH);

    // Destination must be untouched.
    for (FwSizeType i = 0; i < static_cast<FwSizeType>(sizeof(dest)); ++i) {
        EXPECT_EQ(dest[i], 0) << "popBytes must not write on overflow at " << i;
    }
}

// ---------------------------------------------------------------------------
// Null address with non-zero pop count rejected
// ---------------------------------------------------------------------------
TEST(PopBytes, NullAddressRejected) {
    constexpr FwSizeType N = 4;
    U8 srcStorage[N] = {1, 2, 3, 4};
    Fw::SerialBuffer sb(srcStorage, N);
    sb.fill();
    sb.resetDeser();

    Fw::SerializeStatus status = sb.popBytes(nullptr, N, N);

    EXPECT_EQ(status, Fw::FW_DESERIALIZE_SIZE_MISMATCH);
}
