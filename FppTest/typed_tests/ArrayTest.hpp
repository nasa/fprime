// ======================================================================
// \title  ArrayTest.hpp
// \author T. Chieu
// \brief  hpp file for ArrayTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_ARRAY_TEST_HPP
#define FPP_TEST_ARRAY_TEST_HPP

#include "Fw/Types/SerialBuffer.hpp"

#include "STest/Pick/Pick.hpp"
#include "gtest/gtest.h"

#include <sstream>

namespace FppTest {

    namespace Array {

        // Set default values for an array type
        template <typename ArrayType>
        void setDefaultVals
            (typename ArrayType::ElementType (&a)[ArrayType::SIZE]) {}

        // Set test values for an array type
        template <typename ArrayType>
        void setTestVals
            (typename ArrayType::ElementType (&a)[ArrayType::SIZE]);

        template <typename ArrayType>
        ArrayType getMultiElementConstructedArray
            (typename ArrayType::ElementType (&a)[ArrayType::SIZE]);

        // Get the serialized size of an array
        template <typename ArrayType>
        U32 getSerializedSize
            (typename ArrayType::ElementType (&a)[ArrayType::SIZE]) {
            return ArrayType::SERIALIZED_SIZE;
        }

    } // namespace Array

} // namespace FppTest

// Test an array class
template <typename ArrayType>
class ArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        FppTest::Array::setDefaultVals<ArrayType>(defaultVals);

        FppTest::Array::setTestVals<ArrayType>(testVals);
        ASSERT_FALSE(valsAreEqual());
    };

    bool valsAreEqual() {
        for (U32 i = 0; i < ArrayType::SIZE; i++) {
            if (defaultVals[i] != testVals[i]) {
                return false;
            }
        }

        return true;
    }

    typename ArrayType::ElementType defaultVals[ArrayType::SIZE];
    typename ArrayType::ElementType testVals[ArrayType::SIZE];
};

TYPED_TEST_SUITE_P(ArrayTest);

// Test array constants and default constructor
TYPED_TEST_P(ArrayTest, Default) {
    TypeParam a;

    // Constants
    ASSERT_EQ(TypeParam::SIZE, 3);
    ASSERT_EQ(
        TypeParam::SERIALIZED_SIZE,
        TypeParam::SIZE * TypeParam::ElementType::SERIALIZED_SIZE
    );

    // Default constructor
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        ASSERT_EQ(a[i], this->defaultVals[i]);
    }
}

// Test array constructors
TYPED_TEST_P(ArrayTest, Constructors) {
    // Array constructor
    TypeParam a1(this->testVals);
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        ASSERT_EQ(a1[i], this->testVals[i]);
    }

    // Single element constructor
    TypeParam a2(this->testVals[0]);
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        ASSERT_EQ(a2[i], this->testVals[0]);
    }

    // Multiple element constructor
    TypeParam a3 = FppTest::Array::getMultiElementConstructedArray<TypeParam>
        (this->testVals);
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        ASSERT_EQ(a3[i], this->testVals[i]);
    }

    // Copy constructor
    TypeParam a4(a1);
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        ASSERT_EQ(a4[i], a1[i]);
    }
}

// Test array subscript operator
TYPED_TEST_P(ArrayTest, SubscriptOp) {
    TypeParam a;

    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        a[i] = this->testVals[0];
        ASSERT_EQ(a[i], this->testVals[0]);
    }
}

// Test array assignment operator
TYPED_TEST_P(ArrayTest, AssignmentOp) {
    TypeParam a1, a2;

    // Array assignment
    a1 = this->testVals;
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        ASSERT_EQ(a1[i], this->testVals[i]);
    }

    // Copy assignment
    TypeParam& a1Ref = a1;
    a1 = a1Ref;
    ASSERT_EQ(&a1, &a1Ref);

    a1 = a2;
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        ASSERT_EQ(a2[i], a1[i]);
    }

    // Single element assignment
    a1 = this->testVals[0];
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        ASSERT_EQ(a1[i], this->testVals[0]);
    }
}

// Test array equality and inequality operators
TYPED_TEST_P(ArrayTest, EqualityOp) {
    TypeParam a1, a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);

    a2 = this->testVals;

    ASSERT_FALSE(a1 == a2);
    ASSERT_TRUE(a1 != a2);

    a1 = a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);
}

// Test array serialization and deserialization
TYPED_TEST_P(ArrayTest, Serialization) {
    TypeParam a(this->testVals);

    U32 serializedSize = 
        FppTest::Array::getSerializedSize<TypeParam>(this->testVals);
    Fw::SerializeStatus status;

    // Test successful serialization
    TypeParam aCopy;
    U8 data[TypeParam::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize
    status = buf.serialize(a);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(
        buf.getBuffLength(), 
        serializedSize        
    );

    // Deserialize
    status = buf.deserialize(aCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(a, aCopy);

    // Test unsuccessful serialization
    TypeParam aCopy2;
    U8 data2[serializedSize-1];
    Fw::SerialBuffer buf2(data2, sizeof(data2));

    // Serialize
    status = buf2.serialize(a);
    
    ASSERT_NE(status, Fw::FW_SERIALIZE_OK);
    ASSERT_NE(
        buf2.getBuffLength(), 
        serializedSize        
    );

    // Deserialize
    status = buf2.deserialize(aCopy2);

    ASSERT_NE(status, Fw::FW_SERIALIZE_OK);
}

// Register all test patterns
REGISTER_TYPED_TEST_SUITE_P(ArrayTest,
    Default,
    Constructors,
    SubscriptOp,
    AssignmentOp,
    EqualityOp,
    Serialization
);

#endif
