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
#if 0
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(a[i], 0U);
    }
#endif
}

}  // namespace Ds
