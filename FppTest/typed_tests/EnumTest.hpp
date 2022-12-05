// ======================================================================
// \title  EnumTest.hpp
// \author T. Chieu
// \brief  hpp file for EnumTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_ENUM_TEST_HPP
#define FPP_TEST_ENUM_TEST_HPP

#include "Fw/Types/SerialBuffer.hpp"

#include "STest/Pick/Pick.hpp"
#include "gtest/gtest.h"

#include <limits>

namespace FppTest {

    namespace Enum {

        // Get the default value of an enum
        template <typename EnumType>
        typename EnumType::T getDefaultValue() {
            return static_cast<typename EnumType::T>(0);
        }

        // Get a valid value of an enum
        template <typename EnumType>
        typename EnumType::T getValidValue() {
            U32 val = STest::Pick::startLength(
                0, 
                static_cast<U32>(EnumType::NUM_CONSTANTS - 1)
            );

            return static_cast<typename EnumType::T>(val);
        }

        // Get an invalid value of an enum
        template <typename EnumType>
        typename EnumType::T getInvalidValue() {
            U8 sign = 0;
            if (std::numeric_limits<typename EnumType::SerialType>::min() < 0) {
                sign = STest::Pick::lowerUpper(0, 1);
            }

            switch (sign) {
                case 0:
                    return static_cast<typename EnumType::T>(STest::Pick::lowerUpper(
                        EnumType::NUM_CONSTANTS,
                        static_cast<U32>(
                            std::numeric_limits<typename EnumType::SerialType>::max()
                        )
                    ));
                default:
                    return static_cast<typename EnumType::T>(STest::Pick::lowerUpper(
                        1,
                        static_cast<U32>((-1) *
                            (std::numeric_limits<typename EnumType::SerialType>::min() + 1)
                        )
                    ) * (-1));
            }
        }

    } // namespace Enum

} // namespace FppTest

// Test core enum interface
template <typename EnumType>
class EnumTest : public ::testing::Test {};

TYPED_TEST_SUITE_P(EnumTest);

// Test enum constants and default construction
TYPED_TEST_P(EnumTest, Default) {
    TypeParam e;

    // Constants
    ASSERT_EQ(
        TypeParam::SERIALIZED_SIZE, 
        sizeof(typename TypeParam::SerialType)
    );

    // Default constructor
    ASSERT_EQ(e.e, FppTest::Enum::getDefaultValue<TypeParam>());
}

// Test enum constructors
TYPED_TEST_P(EnumTest, Constructors) {
    typename TypeParam::T validVal = FppTest::Enum::getValidValue<TypeParam>();

    // Raw enum value constructor
    TypeParam e1(validVal);
    ASSERT_EQ(e1.e, validVal);

    // Copy constructor
    TypeParam e2(e1);
    ASSERT_EQ(e2.e, validVal);
}

// Test enum assignment operator
TYPED_TEST_P(EnumTest, AssignmentOp) {
    TypeParam e1;
    TypeParam e2;

    typename TypeParam::T validVal = FppTest::Enum::getValidValue<TypeParam>();

    // Raw enum value assignment
    e1 = validVal;
    ASSERT_EQ(e1.e, validVal);
    
    // Object assignment
    e2 = e1;
    ASSERT_EQ(e2.e, validVal);
}

// Test enum equality and inequality operator
TYPED_TEST_P(EnumTest, EqualityOp) {
    // Initialize two distinct valid values
    typename TypeParam::T validVal1 = FppTest::Enum::getValidValue<TypeParam>();
    typename TypeParam::T validVal2 = FppTest::Enum::getValidValue<TypeParam>();
    while (validVal1 == validVal2) {
        validVal2 = FppTest::Enum::getValidValue<TypeParam>();
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
TYPED_TEST_P(EnumTest, IsValidFunction) {
    TypeParam validEnum = FppTest::Enum::getValidValue<TypeParam>();
    TypeParam invalidEnum = FppTest::Enum::getInvalidValue<TypeParam>();

    ASSERT_TRUE(validEnum.isValid());
    ASSERT_FALSE(invalidEnum.isValid());
}

// Test enum serialization and deserialization
TYPED_TEST_P(EnumTest, Serialization) {
    TypeParam validEnum = FppTest::Enum::getValidValue<TypeParam>();
    TypeParam invalidEnum = FppTest::Enum::getInvalidValue<TypeParam>();

    // Copy of enums to test after serialization
    TypeParam validEnumCopy;
    TypeParam invalidEnumCopy;

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
}

// Register all test patterns
REGISTER_TYPED_TEST_SUITE_P(EnumTest,
    Default,
    Constructors,
    AssignmentOp,
    EqualityOp,
    IsValidFunction,
    Serialization
);

#endif
