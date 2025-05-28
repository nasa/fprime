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
}

}  // namespace Ds
