// ======================================================================
// \title  PortBufferTests.cpp
// \brief  Tests verifying port buffer class constructors initialize
//         m_buffAddr and m_capacity via the base class members.
// ======================================================================

#include "FppTest/component/active/NoArgsPortAc.hpp"
#include "FppTest/component/active/PrimitiveArgsPortAc.hpp"
#include "gtest/gtest.h"

TEST(PortBufferTest, ParamPortConstructorInitializesBuffAddr) {
    PrimitiveArgsPortBuffer buf;
    ASSERT_NE(buf.getBuffAddr(), nullptr);
}

TEST(PortBufferTest, ParamPortConstructorInitializesCapacity) {
    PrimitiveArgsPortBuffer buf;
    // local avoids ODR-use of static constexpr in C++14
    constexpr Fw::Serializable::SizeType expected = PrimitiveArgsPortBuffer::CAPACITY;
    ASSERT_EQ(buf.getCapacity(), expected);
    ASSERT_GT(buf.getCapacity(), static_cast<Fw::Serializable::SizeType>(0));
}

TEST(PortBufferTest, NoParamPortHasZeroCapacity) {
    NoArgsPortBuffer buf;
    ASSERT_EQ(buf.getCapacity(), static_cast<Fw::Serializable::SizeType>(0));
}

TEST(PortBufferTest, NoParamPortHasNullBuffAddr) {
    NoArgsPortBuffer buf;
    ASSERT_EQ(buf.getBuffAddr(), nullptr);
}
