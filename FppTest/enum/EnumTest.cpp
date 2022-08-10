#include "FppTest/enum/ImplicitEnumAc.hpp"
#include "FppTest/enum/ExplicitEnumAc.hpp"
#include "FppTest/enum/DefaultEnumAc.hpp"
#include "FppTest/enum/IntervalEnumAc.hpp"
#include "FppTest/enum/SerializeTypeU8EnumAc.hpp"
#include "FppTest/enum/SerializeTypeU64EnumAc.hpp"

#include "Fw/Types/SerialBuffer.hpp"

#include "STest/Pick/Pick.hpp"
#include "gtest/gtest.h"

#include <limits>

// Get the default value of an enum
template <typename EnumType>
typename EnumType::T getDefaultValue() {
    return EnumType::A;
}

template<>
Default::T getDefaultValue<Default>() {
    return Default::C;
}

// Get a valid value of an enum
template <typename EnumType>
typename EnumType::T getValidValue() {
    U32 val = STest::Pick::startLength(0, EnumType::NUM_CONSTANTS);

    switch (val) {
        case 0: return EnumType::A;
        case 1: return EnumType::B;
        default: return EnumType::C;
    }
}

template<>
Interval::T getValidValue<Interval>() {
    U32 val = STest::Pick::startLength(0, Interval::NUM_CONSTANTS);

    switch (val) {
        case 0: return Interval::A;
        case 1: return Interval::B;
        case 2: return Interval::C;
        case 3: return Interval::D;
        case 4: return Interval::E;
        case 5: return Interval::F;
        default: return Interval::G;
    }
}

// Get an invalid value of an enum
template <typename EnumType>
typename EnumType::T getInvalidValue() {
    U32 sign = STest::Pick::lowerUpper(0, 1);

    switch (sign) {
        case 0:
            return static_cast<typename EnumType::T>(STest::Pick::lowerUpper(
                EnumType::B + 1,
                EnumType::C - 1
            ));
        default:
            return static_cast<typename EnumType::T>(STest::Pick::lowerUpper(
                0,
                EnumType::A - 1
            ) * (-1));
    }
}

template<>
Implicit::T getInvalidValue<Implicit>() {
    return static_cast<Implicit::T>(STest::Pick::lowerUpper(
        Implicit::C + 1, 
        std::numeric_limits<Implicit::SerialType>::max()
    ));
}

template<>
Interval::T getInvalidValue<Interval>() {
    return static_cast<Interval::T>(STest::Pick::lowerUpper(
        Interval::G + 1,
        std::numeric_limits<Interval::SerialType>::max()
    ));
}

template<>
SerializeTypeU8::T getInvalidValue<SerializeTypeU8>() {
    return static_cast<SerializeTypeU8::T>(STest::Pick::lowerUpper(
        SerializeTypeU8::C + 1,
        SerializeTypeU8::A - 1
    ));
}

template<>
SerializeTypeU64::T getInvalidValue<SerializeTypeU64>() {
    return static_cast<SerializeTypeU64::T>(STest::Pick::lowerUpper(
        SerializeTypeU64::B + 1,
        SerializeTypeU64::A - 1
    ));
}

// Test core enum interface
template <typename EnumType>
class EnumTest : public ::testing::Test {};

// Specify type parameters for this test suite
using EnumTypes = ::testing::Types<
    Implicit, 
    Explicit, 
    Default,
    Interval, 
    SerializeTypeU8,
    SerializeTypeU64
>;
TYPED_TEST_SUITE(EnumTest, EnumTypes);

// Test enum constants and default construction
TYPED_TEST(EnumTest, Default) {
    TypeParam e;

    // Constants
    ASSERT_EQ(
        TypeParam::SERIALIZED_SIZE, 
        sizeof(typename TypeParam::SerialType)
    );

    // Default constructor
    ASSERT_EQ(e.e, getDefaultValue<TypeParam>());
}

// Test enum constructors
TYPED_TEST(EnumTest, Constructors) {
    typename TypeParam::T validVal = getValidValue<TypeParam>();

    // Raw enum value constructor
    TypeParam e1(validVal);
    ASSERT_EQ(e1.e, validVal);

    // Copy constructor
    TypeParam e2(e1);
    ASSERT_EQ(e2.e, validVal);
}

// Test enum assignment operator
TYPED_TEST(EnumTest, AssignmentOp) {
    TypeParam e1;
    TypeParam e2;

    typename TypeParam::T validVal = getValidValue<TypeParam>();

    // Raw enum value assignment
    e1 = validVal;
    ASSERT_EQ(e1.e, validVal);
    
    // Object assignment
    e2 = e1;
    ASSERT_EQ(e2.e, validVal);
}

// Test enum equality and inequality operator
TYPED_TEST(EnumTest, EqualityOp) {
    // Initialize two distinct valid values
    typename TypeParam::T validVal1 = getValidValue<TypeParam>();
    typename TypeParam::T validVal2 = getValidValue<TypeParam>();
    while (validVal1 == validVal2) {
        validVal2 = getValidValue<TypeParam>();
    }

    TypeParam e1;
    TypeParam e2;
    TypeParam e3(validVal1);
    TypeParam e4(validVal2);

    // operator==
    ASSERT_TRUE(e3 == validVal1);
    ASSERT_TRUE(e4 == validVal2);
    ASSERT_FALSE(e3 == validVal2);
    ASSERT_FALSE(e4 == validVal1);

    ASSERT_TRUE(e1 == e2);
    ASSERT_FALSE(e3 == e4);

    // operator!=
    ASSERT_TRUE(e3 != validVal2);
    ASSERT_TRUE(e4 != validVal1);
    ASSERT_FALSE(e3 != validVal1);
    ASSERT_FALSE(e4 != validVal2);

    ASSERT_TRUE(e3 != e4);
    ASSERT_FALSE(e1 != e2);
}

// Test enum isValid() function
TYPED_TEST(EnumTest, IsValidFunction) {
    TypeParam validEnum = getValidValue<TypeParam>();
    TypeParam invalidEnum = getInvalidValue<TypeParam>();

    ASSERT_TRUE(validEnum.isValid());
    ASSERT_FALSE(invalidEnum.isValid());
}

// Test enum serialization and deserialization
TYPED_TEST(EnumTest, Serialization) {
    TypeParam validEnum = getValidValue<TypeParam>();
    TypeParam invalidEnum = getInvalidValue<TypeParam>();

    // Copy of enums to test after serialization
    TypeParam validEnumCopy = validEnum;
    TypeParam invalidEnumCopy = invalidEnum;

    Fw::SerializeStatus status;
    U8 data[TypeParam::SERIALIZED_SIZE * 2];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize the enums
    status = buf.serialize(validEnum);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getBuffLength(), sizeof(typename TypeParam::SerialType));

    status = buf.serialize(invalidEnum);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getBuffLength(), sizeof(typename TypeParam::SerialType) * 2);

    // Deserialize the enums
    status = buf.deserialize(validEnumCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(validEnumCopy, validEnum);

    status = buf.deserialize(invalidEnumCopy);

    ASSERT_EQ(status, Fw::FW_DESERIALIZE_FORMAT_ERROR);
    ASSERT_EQ(invalidEnumCopy, invalidEnum);
}
