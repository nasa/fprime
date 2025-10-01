// ======================================================================
// \title  CircularIndexTest.cpp
// \author bocchino
// \brief  Tests for CircularIndex types
// ======================================================================

#include <gtest/gtest.h>
#include <limits>

#include "Fw/DataStructures/CircularIndex.hpp"
#include "STest/Pick/Pick.hpp"

namespace Fw {

namespace {

FwSizeType pickValue() {
    return static_cast<FwSizeType>(STest::Pick::any());
}

FwSizeType pickModulus() {
    return static_cast<FwSizeType>(STest::Pick::lowerUpper(1, std::numeric_limits<U32>::max()));
}

}  // namespace

TEST(CircularIndex, ZeroArgConstructor) {
    CircularIndex ci;
    ASSERT_EQ(ci.getValue(), 0);
    ASSERT_EQ(ci.getModulus(), 1);
}

TEST(CircularIndex, SpecifiedMemberConstructor) {
    const FwSizeType modulus = pickModulus();
    const FwSizeType value = pickValue();
    CircularIndex ci(modulus, value);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_EQ(ci.getValue(), value % modulus);
}

TEST(CircularIndex, CopyConstructor) {
    const FwSizeType modulus = pickModulus();
    const FwSizeType value = pickValue();
    CircularIndex ci1(modulus, value);
    CircularIndex ci2(ci1);
    ASSERT_EQ(ci1.getModulus(), ci2.getModulus());
    ASSERT_EQ(ci1.getValue(), ci2.getValue());
}

TEST(CircularIndex, CopyAssignment) {
    const FwSizeType modulus = pickModulus();
    const FwSizeType value = pickValue();
    CircularIndex ci1(modulus, value);
    CircularIndex ci2;
    ci2 = ci1;
    ASSERT_EQ(ci1.getModulus(), ci2.getModulus());
    ASSERT_EQ(ci1.getValue(), ci2.getValue());
}

TEST(CircularIndex, SetValue) {
    const FwSizeType modulus = pickModulus();
    const FwSizeType value = pickValue();
    CircularIndex ci(modulus);
    ASSERT_EQ(ci.getValue(), 0);
    ASSERT_EQ(ci.getModulus(), modulus);
    ci.setValue(value);
    ASSERT_EQ(ci.getValue(), value % modulus);
}

TEST(CircularIndex, SetModulus) {
    const FwSizeType value = pickValue();
    const FwSizeType modulus = pickModulus();
    CircularIndex ci(modulus, value);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_EQ(ci.getValue(), value % modulus);
    ci.setModulus(modulus);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_EQ(ci.getValue(), value % modulus);
}

TEST(CircularIndex, IncrementOne) {
    const FwSizeType value = pickValue();
    const FwSizeType modulus = pickModulus();
    CircularIndex ci(modulus, value);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_EQ(ci.getValue(), value % modulus);
    ci.increment();
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_EQ(ci.getValue(), (value + 1) % modulus);
}

TEST(CircularIndex, IncrementRandom) {
    const FwSizeType value = pickValue();
    const FwSizeType modulus = pickModulus();
    const FwSizeType amount = pickValue();
    const FwSizeType offset = amount % modulus;
    CircularIndex ci(modulus, value);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_EQ(ci.getValue(), value % modulus);
    ci.increment(amount);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_GT(modulus, 0);
    ASSERT_EQ(ci.getValue(), (value + offset) % modulus);
}

TEST(CircularIndex, DecrementOne) {
    const FwSizeType value = pickValue();
    const FwSizeType modulus = pickModulus();
    CircularIndex ci(modulus, value);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_EQ(ci.getValue(), value % modulus);
    ci.decrement();
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_GT(modulus, 0);
    ASSERT_GE(value + modulus, 1);
    ASSERT_EQ(ci.getValue(), ((value + modulus) - 1) % modulus);
}

TEST(CircularIndex, DecrementRandom) {
    const FwSizeType value = pickValue();
    const FwSizeType modulus = pickModulus();
    const FwSizeType amount = pickValue();
    const FwSizeType offset = amount % modulus;
    CircularIndex ci(modulus, value);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_EQ(ci.getValue(), value % modulus);
    ci.decrement(amount);
    ASSERT_EQ(ci.getModulus(), modulus);
    ASSERT_GT(modulus, 0);
    ASSERT_GE(value + modulus, offset);
    ASSERT_EQ(ci.getValue(), (value + modulus - offset) % modulus);
}
}  // namespace Fw
