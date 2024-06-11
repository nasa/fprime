// ======================================================================
// \title  ASTERIA/Types/GTest/Bytes.cpp
// \author bocchino
// \brief  cpp file for Bytes
//
// \copyright
// Copyright (C) 2016, California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/Types/GTest/Bytes.hpp>

namespace Fw {

namespace GTest {

void Bytes ::compare(const Bytes& expected, const Bytes& actual) {
    ASSERT_EQ(expected.size, actual.size);
    for (size_t i = 0; i < expected.size; ++i)
        ASSERT_EQ(expected.bytes[i], actual.bytes[i]) << "At i=" << i << "\n";
}

}  // namespace GTest

}  // namespace Fw
