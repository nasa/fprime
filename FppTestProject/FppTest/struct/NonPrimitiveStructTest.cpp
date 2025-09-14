// ======================================================================
// \title  NonPrimitiveStructTest.cpp
// \author T. Chieu
// \brief  cpp file for NonPrimitiveStructTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/struct/NonPrimitiveSerializableAc.hpp"
#include "FppTest/utils/Utils.hpp"

#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/StringTemplate.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "STest/Pick/Pick.hpp"

#include "gtest/gtest.h"

#include <sstream>

// Test NonPrimitive struct class
class NonPrimitiveStructTest : public ::testing::Test {
  protected:
    void SetUp() override {
        char buf[testString.getCapacity()];
        // Test string must be non-empty
        FppTest::Utils::setString(buf, sizeof(buf), 1);
        testString = buf;

        testEnum = static_cast<StructEnum::T>(STest::Pick::startLength(StructEnum::A, StructEnum::B));

        for (U32 i = 0; i < StructArray::SIZE; i++) {
            testArray[i] = FppTest::Utils::getNonzeroU32();
        }

        testStruct.set(true, FppTest::Utils::getNonzeroU32(), static_cast<I16>(FppTest::Utils::getNonzeroU32()),
                       static_cast<F64>(FppTest::Utils::getNonzeroU32()));

        for (U32 i = 0; i < 3; i++) {
            testU32Arr[0] = FppTest::Utils::getNonzeroU32();
        }

        for (U32 i = 0; i < 3; i++) {
            testStructArr[i].set(true, FppTest::Utils::getNonzeroU32(),
                                 static_cast<I16>(FppTest::Utils::getNonzeroU32()),
                                 static_cast<F64>(FppTest::Utils::getNonzeroU32()));
        }
    }

    void assertStructMembers(const NonPrimitive& s) {
        ASSERT_EQ(s.get_mString(), testString);
        ASSERT_EQ(s.get_mEnum(), testEnum);
        ASSERT_EQ(s.get_mArray(), testArray);
        ASSERT_EQ(s.get_mStruct(), testStruct);

        for (U32 i = 0; i < 3; i++) {
            ASSERT_EQ(s.get_mU32Arr()[i], testU32Arr[i]);
        }
        for (U32 i = 0; i < 3; i++) {
            ASSERT_EQ(s.get_mStructArr()[i], testStructArr[i]);
        }
    }

    void assertUnsuccessfulSerialization(NonPrimitive& s, FwSizeType bufSize) {
        U8 data[bufSize];
        Fw::SerialBuffer buf(data, sizeof(data));
        Fw::SerializeStatus status;

        // Serialize
        status = buf.serialize(s);
        ASSERT_NE(status, Fw::FW_SERIALIZE_OK);

        // Deserialize
        status = buf.deserialize(s);
        ASSERT_NE(status, Fw::FW_SERIALIZE_OK);
    }

    Fw::StringTemplate<80> testString;
    StructEnum testEnum;
    StructArray testArray;
    Primitive testStruct;
    U32 testU32Arr[3];
    Primitive testStructArr[3];
};

// Test struct constants and default constructor
TEST_F(NonPrimitiveStructTest, Default) {
    NonPrimitive s;

    StructArray defaultArray;
    Primitive defaultStruct1(true, 0, 0, 3.14);
    Primitive defaultStruct2(true, 0, 0, 1.16);

    // Constants
    ASSERT_EQ(NonPrimitive::SERIALIZED_SIZE, Fw::StringBase::STATIC_SERIALIZED_SIZE(80) + StructEnum::SERIALIZED_SIZE +
                                                 StructArray::SERIALIZED_SIZE + StructArrAlias::SERIALIZED_SIZE +
                                                 Primitive::SERIALIZED_SIZE + StructSAlias::SERIALIZED_SIZE +
                                                 (3 * sizeof(U32)) + (3 * Primitive::SERIALIZED_SIZE));

    // Default constructor
    ASSERT_EQ(s.get_mString(), "");
    ASSERT_EQ(s.get_mEnum(), StructEnum::C);
    ASSERT_EQ(s.get_mArray(), defaultArray);
    ASSERT_EQ(s.get_mStruct(), defaultStruct1);

    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s.get_mU32Arr()[i], 0);
    }
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s.get_mStructArr()[i], defaultStruct2);
    }
}

// Test struct constructors
TEST_F(NonPrimitiveStructTest, Constructors) {
    // Member constructor
    NonPrimitive s1(testString, testEnum, testArray, testArray, testStruct, testStruct, testU32Arr, testStructArr);
    assertStructMembers(s1);

    // Scalar member constructor
    NonPrimitive s2(testString, testEnum, testArray, testArray, testStruct, testStruct, testU32Arr[0],
                    testStructArr[0]);

    ASSERT_EQ(s2.get_mString(), testString);
    ASSERT_EQ(s2.get_mEnum(), testEnum);
    ASSERT_EQ(s2.get_mArray(), testArray);
    ASSERT_EQ(s2.get_mStruct(), testStruct);

    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s2.get_mU32Arr()[i], testU32Arr[0]);
    }
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s2.get_mStructArr()[i], testStructArr[0]);
    }

    // Copy constructor
    NonPrimitive s3(s1);
    assertStructMembers(s3);
}

// Test struct assignment operator
TEST_F(NonPrimitiveStructTest, AssignmentOp) {
    NonPrimitive s1;
    NonPrimitive s2(testString, testEnum, testArray, testArray, testStruct, testStruct, testU32Arr, testStructArr);

    // Copy assignment
    s1 = s2;
    assertStructMembers(s1);

    NonPrimitive& s1Ref = s1;
    s1 = s1Ref;
    ASSERT_EQ(&s1, &s1Ref);
}

// Test struct equality and inequality operators
TEST_F(NonPrimitiveStructTest, EqualityOp) {
    NonPrimitive s1, s2;

    ASSERT_TRUE(s1 == s2);
    ASSERT_FALSE(s1 != s2);

    s1.set_mString(testString);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mString(testString);
    s1.set_mEnum(testEnum);

    ASSERT_NE(s1, s2);
    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mEnum(testEnum);
    s1.set_mArray(testArray);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mArray(testArray);
    s1.set_mStruct(testStruct);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mStruct(testStruct);
    s1.set_mU32Arr(testU32Arr);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mU32Arr(testU32Arr);
    s1.set_mStructArr(testStructArr);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mStructArr(testStructArr);

    ASSERT_TRUE(s1 == s2);
    ASSERT_FALSE(s1 != s2);
}

// Test struct getter and setter functions
TEST_F(NonPrimitiveStructTest, GetterSetterFunctions) {
    NonPrimitive s1, s2;

    // Set all members
    s1.set(testString, testEnum, testArray, testArray, testStruct, testStruct, testU32Arr, testStructArr);
    assertStructMembers(s1);

    // Set individual members
    s2.set_mString(testString);
    ASSERT_EQ(s2.get_mString(), testString);

    s2.set_mEnum(testEnum);
    ASSERT_EQ(s2.get_mEnum(), testEnum);

    s2.set_mArray(testArray);
    ASSERT_EQ(s2.get_mArray(), testArray);

    s2.set_mStruct(testStruct);
    ASSERT_EQ(s2.get_mStruct(), testStruct);

    s2.set_mU32Arr(testU32Arr);
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s2.get_mU32Arr()[i], testU32Arr[i]);
    }

    s2.set_mStructArr(testStructArr);
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s2.get_mStructArr()[i], testStructArr[i]);
    }

    // Check non-const getter
    s2.get_mStruct().set_mU32(testU32Arr[0]);
    ASSERT_EQ(s2.get_mStruct().get_mU32(), testU32Arr[0]);
}

// Test struct serialization and deserialization
TEST_F(NonPrimitiveStructTest, Serialization) {
    NonPrimitive s(testString, testEnum, testArray, testArray, testStruct, testStruct, testU32Arr, testStructArr);
    NonPrimitive sCopy;

    U32 stringSerializedSize = static_cast<U32>(testString.length() + sizeof(FwBuffSizeType));
    U32 serializedSize = static_cast<U32>(NonPrimitive::SERIALIZED_SIZE - Fw::StringBase::STATIC_SERIALIZED_SIZE(80) +
                                          stringSerializedSize);
    Fw::SerializeStatus status;

    // Test successful serialization
    U8 data[NonPrimitive::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize
    status = buf.serialize(s);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getBuffLength(), serializedSize);

    // Deserialize
    status = buf.deserialize(sCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(s, sCopy);

    // Test unsuccessful serialization
    assertUnsuccessfulSerialization(s, stringSerializedSize - 1);
    assertUnsuccessfulSerialization(s, stringSerializedSize + StructEnum::SERIALIZED_SIZE - 1);
    assertUnsuccessfulSerialization(
        s, stringSerializedSize + StructEnum::SERIALIZED_SIZE + StructArray::SERIALIZED_SIZE - 1);
    assertUnsuccessfulSerialization(s, stringSerializedSize + StructEnum::SERIALIZED_SIZE +
                                           StructArray::SERIALIZED_SIZE + Primitive::SERIALIZED_SIZE - 1);
    assertUnsuccessfulSerialization(s, stringSerializedSize + StructEnum::SERIALIZED_SIZE +
                                           StructArray::SERIALIZED_SIZE + Primitive::SERIALIZED_SIZE +
                                           (3 * sizeof(U32)) - 1);
    assertUnsuccessfulSerialization(s, serializedSize - 1);
}

// Test struct toString() and ostream operator functions
TEST_F(NonPrimitiveStructTest, ToString) {
    NonPrimitive s(testString, testEnum, testArray, testArray, testStruct, testStruct, testU32Arr, testStructArr);
    std::stringstream buf1, buf2;

    buf1 << s;

    buf2 << "( "
         << "mString = " << testString << ", "
         << "mEnum = " << testEnum << ", "
         << "mArray = " << testArray << ", "
         << "mAliasStructAlias = " << testArray << ", "
         << "mStruct = " << testStruct << ", "
         << "mAliasStruct = " << testStruct << ", "
         << "mU32Arr = [ " << testU32Arr[0] << ", " << testU32Arr[1] << ", " << testU32Arr[2] << " ], "
         << "mStructArr = [ " << testStructArr[0] << ", " << testStructArr[1] << ", " << testStructArr[2] << " ] "
         << " )";

    // Truncate string output
    Fw::String s2(buf2.str().c_str());

    ASSERT_STREQ(buf1.str().c_str(), s2.toChar());
}
