// ======================================================================
// \title  ArrayTest.cpp
// \author bocchino
// \brief  cpp file for Array tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/Array.hpp"

namespace Fw {

TEST(Array, ZeroArgConstructor) {
    Array<U32, 3> a;
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(a[i], 0U);
    }
}

TEST(Array, InitializerListConstructor) {
    // Explicit call to constructor
    Array<U32, 3> a({1, 2, 3});
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(a[i], i + 1);
    }
    // Implicit call to constructor in assignment
    Array<U32, 3> b = {1, 2, 3};
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(b[i], i + 1);
    }
}

TEST(Array, RawArrayConstructor) {
    // Explicit call to constructor
    U32 elements[3] = {1, 2, 3};
    Array<U32, 3> a(elements);
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(a[i], i + 1);
    }
    // Implicit call to constructor in assignment
    Array<U32, 3> b = elements;
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(b[i], i + 1);
    }
}

TEST(Array, SingleElementConstructor) {
    // Explicit call to constructor in variable declaration
    Array<U32, 3> a(1);
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(a[i], 1);
    }
    // Explicit call to constructor in assignment
    Array<U32, 3> b = Array<U32, 3>(2);
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(b[i], 2);
    }
}

TEST(Array, CopyConstructor) {
    // Call the single-item constructor
    Array<U32, 3> a1(10);
    // Call the copy constructor
    Array<U32, 3> a2(a1);
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(a2[i], 10);
    }
}

TEST(Array, Subscript) {
    Array<U32, 3> a = {0, 1, 2};
    // Constant access
    ASSERT_EQ(a[1], 1);
    // Mutable access
    a[1]++;
    ASSERT_EQ(a[1], 2);
    // Out-of-bounds access
    ASSERT_DEATH(a[3], "Assert");
}

TEST(Array, CopyAssignmentOperator) {
    Array<U32, 3> a1(1);
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(a1[i], 1);
    }
    Array<U32, 3> a2(2);
    auto& a = (a1 = a2);
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(a1[i], 2);
    }
    ASSERT_EQ(&a, &a1);
}

TEST(Array, GetElements) {
    Array<U32, 3> a;
    // Mutable reference
    auto& elements1 = a.getElements();
    ASSERT_EQ(elements1[0], 0);
    elements1[0] = 1;
    // Constant reference
    const auto& elements2 = a.getElements();
    ASSERT_EQ(elements2[0], 1);
}

TEST(Array, AsExternalArray) {
    Array<U32, 3> a = {1, 2, 3};
    ExternalArray<U32> ea = a.asExternalArray();
    ASSERT_EQ(ea[0], 1);
}

}  // namespace Fw
