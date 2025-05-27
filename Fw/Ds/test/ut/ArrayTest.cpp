// ======================================================================
// \title  ArrayTest.cpp
// \author bocchino
// \brief  cpp file for Array tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/Ds/Array.hpp"

namespace Ds {

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
    // Implicit call to constructor via initialization
    Array<U32, 3> b = {1, 2, 3};
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

}  // namespace Ds
