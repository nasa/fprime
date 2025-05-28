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

}  // namespace Ds
