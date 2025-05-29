// ======================================================================
// \title  ExternalArrayTest.cpp
// \author bocchino
// \brief  cpp file for ExternalArray tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/Ds/ExternalArray.hpp"

namespace Ds {

TEST(ExternalArray, ZeroArgConstructor) {
    ExternalArray<U32> a;
    ASSERT_EQ(a.getElements(), nullptr);
    ASSERT_EQ(a.getSize(), 0);
}

TEST(ExternalArray, StorageConstructor) {
    U32 elements[3];
    ExternalArray<U32> a(elements, 3);
    ASSERT_EQ(a.getElements(), elements);
    ASSERT_EQ(a.getSize(), 3);
}

TEST(ExternalArray, CopyConstructor) {
    U32 elements[3];
    // Call the constructor providing backing storage
    ExternalArray<U32> a1(elements, 3);
    // Call the copy constructor
    ExternalArray<U32> a2(a1);
    ASSERT_EQ(a2.getElements(), elements);
    ASSERT_EQ(a2.getSize(), 3);
}

TEST(ExternalArray, CopyAssignment) {
    U32 elements[3];
    // Call the constructor providing backing storage
    ExternalArray<U32> a1(elements, 3);
    // Call the copy assignment operator
    ExternalArray<U32> a2;
    a2 = a1;
    ASSERT_EQ(a1.getElements(), a2.getElements());
    ASSERT_EQ(a1.getSize(), a2.getSize());
}

static void testCopyFrom(
    ExternalArray<U32> a1,
    ExternalArray<U32> a2
) {
    const FwSizeType size1 = a1.getSize();
    for (FwSizeType i = 0; i < size1; i++) {
        a1[i] = static_cast<U32>(i);
    }
    const FwSizeType size2 = a2.getSize();
    for (FwSizeType i = 0; i < size2; i++) {
        a2[i] = 0;
    }
    a2.copyFrom(a1);
    const FwSizeType size = FW_MIN(size1, size2);
    for (FwSizeType i = 0; i < size; i++) {
      ASSERT_EQ(a2[i], a1[i]);
    }
}

TEST(ExternalArray, CopyFrom) {
    constexpr FwSizeType maxSize = 10;
    U32 elements1[maxSize];
    U32 elements2[maxSize];
    // size1 < size2
    testCopyFrom(ExternalArray<U32>(elements1, 5), ExternalArray<U32>(elements2, 10));
    // size1 == size2
    testCopyFrom(ExternalArray<U32>(elements1, 10), ExternalArray<U32>(elements2, 10));
    // size1 > size2
    testCopyFrom(ExternalArray<U32>(elements1, 10), ExternalArray<U32>(elements2, 5));
}

TEST(ExternalArray, Subscript) {
    U32 elements[3] = {};
    ExternalArray<U32> a(elements, 3);
    // Constant access
    ASSERT_EQ(a[0], 0);
    // Mutable access
    a[0]++;
    ASSERT_EQ(a[0], 1);
    // Out-of-bounds access
    ASSERT_DEATH(a[3], "Assert");
}

TEST(ExternalArray, SetStorage) {
    U32 elements[3];
    ExternalArray<U32> a1(elements, 3);
    ExternalArray<U32> a2;
    a2.setStorage(a1.getElements(), a1.getSize());
    ASSERT_EQ(a2.getElements(), a1.getElements());
    ASSERT_EQ(a2.getSize(), a1.getSize());
}

}  // namespace Ds
