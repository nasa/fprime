// ======================================================================
// \title  ExternalArrayTest.cpp
// \author bocchino
// \brief  cpp file for ExternalArray tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/Buffer/Buffer.hpp"
#include "Fw/DataStructures/ExternalArray.hpp"

namespace Fw {

TEST(ExternalArray, ZeroArgConstructor) {
    ExternalArray<U32> a;
    ASSERT_EQ(a.getElements(), nullptr);
    ASSERT_EQ(a.getSize(), 0);
}

TEST(ExternalArray, StorageConstructorTyped) {
    constexpr FwSizeType size = 3;
    U32 elements[size];
    ExternalArray<U32> a(elements, size);
    ASSERT_EQ(a.getElements(), elements);
    ASSERT_EQ(a.getSize(), size);
}

TEST(ExternalArray, StorageConstructorUntyped) {
    constexpr FwSizeType size = 3;
    constexpr U8 alignment = ExternalArray<Fw::Buffer>::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = ExternalArray<Fw::Buffer>::getByteArraySize(size);
    alignas(alignment) U8 bytes[byteArraySize];
    ExternalArray<Fw::Buffer> a(ByteArray(&bytes[0], sizeof bytes), size);
    ASSERT_EQ(a.getElements(), reinterpret_cast<Fw::Buffer*>(&bytes[0]));
    ASSERT_EQ(a.getSize(), size);
    a[0] = Fw::Buffer();
}

TEST(ExternalArray, CopyConstructor) {
    constexpr FwSizeType size = 3;
    U32 elements[size];
    // Call the constructor providing backing storage
    ExternalArray<U32> a1(elements, size);
    // Call the copy constructor
    ExternalArray<U32> a2(a1);
    ASSERT_EQ(a2.getElements(), elements);
    ASSERT_EQ(a2.getSize(), size);
}

TEST(ExternalArray, CopyAssignment) {
    constexpr FwSizeType size = 3;
    U32 elements[size];
    // Call the constructor providing backing storage
    ExternalArray<U32> a1(elements, size);
    // Call the copy assignment operator
    ExternalArray<U32> a2;
    a2 = a1;
    ASSERT_EQ(a1.getElements(), a2.getElements());
    ASSERT_EQ(a1.getSize(), a2.getSize());
}

namespace {

void testCopyDataFrom(ExternalArray<U32> a1, ExternalArray<U32> a2) {
    const FwSizeType size1 = a1.getSize();
    for (FwSizeType i = 0; i < size1; i++) {
        a1[i] = static_cast<U32>(i);
    }
    const FwSizeType size2 = a2.getSize();
    for (FwSizeType i = 0; i < size2; i++) {
        a2[i] = 0;
    }
    a2.copyDataFrom(a1);
    const FwSizeType size = FW_MIN(size1, size2);
    for (FwSizeType i = 0; i < size; i++) {
        ASSERT_EQ(a2[i], a1[i]);
    }
}

}  // namespace

TEST(ExternalArray, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    U32 elements1[maxSize];
    U32 elements2[maxSize];
    // size1 < size2
    testCopyDataFrom(ExternalArray<U32>(elements1, smallSize), ExternalArray<U32>(elements2, maxSize));
    // size1 == size2
    testCopyDataFrom(ExternalArray<U32>(elements1, maxSize), ExternalArray<U32>(elements2, maxSize));
    // size1 > size2
    testCopyDataFrom(ExternalArray<U32>(elements1, maxSize), ExternalArray<U32>(elements2, smallSize));
}

TEST(ExternalArray, Subscript) {
    constexpr FwSizeType size = 10;
    U32 elements[size] = {};
    ExternalArray<U32> a(elements, size);
    // Constant access
    ASSERT_EQ(a[0], 0);
    // Mutable access
    a[0]++;
    ASSERT_EQ(a[0], 1);
    // Out-of-bounds access
    ASSERT_DEATH(a[size], "Assert");
}

TEST(ExternalArray, SetStorageTyped) {
    constexpr FwSizeType size = 10;
    U32 elements[size];
    ExternalArray<U32> a;
    a.setStorage(elements, size);
    ASSERT_EQ(a.getElements(), elements);
    ASSERT_EQ(a.getSize(), size);
}

TEST(ExternalArray, SetStorageUntypedOK) {
    constexpr FwSizeType size = 10;
    constexpr U8 alignment = ExternalArray<U32>::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = ExternalArray<U32>::getByteArraySize(size);
    alignas(alignment) U8 bytes[byteArraySize];
    ExternalArray<U32> a;
    a.setStorage(ByteArray(&bytes[0], sizeof bytes), size);
    ASSERT_EQ(a.getElements(), reinterpret_cast<U32*>(bytes));
    ASSERT_EQ(a.getSize(), size);
}

TEST(ExternalArray, SetStorageUntypedBadSize) {
    constexpr FwSizeType size = 10;
    constexpr U8 alignment = ExternalArray<U32>::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = ExternalArray<U32>::getByteArraySize(size);
    alignas(alignment) U8 bytes[byteArraySize];
    ExternalArray<U32> a;
    ASSERT_DEATH(a.setStorage(ByteArray(&bytes[0], sizeof bytes), size + 1), "Assert");
}

TEST(ExternalArray, SetStorageUntypedBadAlignment) {
    constexpr FwSizeType size = 10;
    constexpr U8 alignment = ExternalArray<U32>::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = ExternalArray<U32>::getByteArraySize(size);
    alignas(alignment) U8 bytes[byteArraySize];
    ExternalArray<U32> a;
    ASSERT_DEATH(a.setStorage(ByteArray(&bytes[1], sizeof bytes), size), "Assert");
}

}  // namespace Fw
