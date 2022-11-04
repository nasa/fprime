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
#include "Fw/Types/StringUtils.hpp"
#include "STest/Pick/Pick.hpp"

#include "gtest/gtest.h"

#include <sstream>

// Test NonPrimitive struct class
class NonPrimitiveStructTest : public ::testing::Test {
protected:
    void SetUp() override {
        char buf[testString.getCapacity()];
        FppTest::Utils::setString(buf, sizeof(buf));
        testString = buf;

        testEnum = static_cast<StructEnum::T>(STest::Pick::startLength(
           StructEnum::A,
           StructEnum::B
        ));

        for (U32 i = 0; i < StructArray::SIZE; i++) {
            testArray[i] = FppTest::Utils::getU32();
        }

        testStruct.set(
            true,
            FppTest::Utils::getU32(),
            static_cast<I16>(FppTest::Utils::getU32()),
            static_cast<F64>(FppTest::Utils::getU32())
        );

        for (U32 i = 0; i < 3; i++) {
            testU32Arr[0] = FppTest::Utils::getU32();
        }

        for (U32 i = 0; i < 3; i++) {
            testStructArr[i].set(
                true,
                FppTest::Utils::getU32(),
                static_cast<I16>(FppTest::Utils::getU32()),
                static_cast<F64>(FppTest::Utils::getU32())
            );
        }
    }

    void assertStructMembers(const NonPrimitive& s) {
        ASSERT_EQ(s.getmString(), testString);
        ASSERT_EQ(s.getmEnum(), testEnum);
        ASSERT_EQ(s.getmArray(), testArray);
        ASSERT_EQ(s.getmStruct(), testStruct);

        for (U32 i = 0; i < 3; i++) {
            ASSERT_EQ(s.getmU32Arr()[i], testU32Arr[i]);
        }
        for (U32 i = 0; i < 3; i++) {
            ASSERT_EQ(s.getmStructArr()[i], testStructArr[i]);
        }
    }

    void assertUnsuccessfulSerialization(NonPrimitive& s, U32 bufSize) {
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

    NonPrimitive::StringSize80 testString;
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
    ASSERT_EQ(
        NonPrimitive::SERIALIZED_SIZE,
        NonPrimitive::StringSize80::SERIALIZED_SIZE 
            + StructEnum::SERIALIZED_SIZE
            + StructArray::SERIALIZED_SIZE
            + Primitive::SERIALIZED_SIZE
            + (3 * sizeof(U32))
            + (3 * Primitive::SERIALIZED_SIZE)
    );

    // Default constructor
    ASSERT_EQ(s.getmString(), "");
    ASSERT_EQ(s.getmEnum(), StructEnum::C);
    ASSERT_EQ(s.getmArray(), defaultArray);
    ASSERT_EQ(s.getmStruct(), defaultStruct1);

    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s.getmU32Arr()[i], 0);
    }
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s.getmStructArr()[i], defaultStruct2);
    }
}

// Test struct constructors
TEST_F(NonPrimitiveStructTest, Constructors) {
    // Member constructor
    NonPrimitive s1(testString, testEnum, testArray, 
                    testStruct, testU32Arr, testStructArr);
    assertStructMembers(s1);

    // Scalar member constructor
    NonPrimitive s2(testString, testEnum, testArray, 
                    testStruct, testU32Arr[0], testStructArr[0]);

    ASSERT_EQ(s2.getmString(), testString);
    ASSERT_EQ(s2.getmEnum(), testEnum);
    ASSERT_EQ(s2.getmArray(), testArray);
    ASSERT_EQ(s2.getmStruct(), testStruct);

    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s2.getmU32Arr()[i], testU32Arr[0]);
    }
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s2.getmStructArr()[i], testStructArr[0]);
    }

    // Copy constructor
    NonPrimitive s3(s1);
    assertStructMembers(s3);
}

// Test struct assignment operator
TEST_F(NonPrimitiveStructTest, AssignmentOp) {
    NonPrimitive s1;
    NonPrimitive s2(testString, testEnum, testArray, 
                    testStruct, testU32Arr, testStructArr);

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

    s1.setmString(testString);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmString(testString);
    s1.setmEnum(testEnum);

    ASSERT_NE(s1, s2);
    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmEnum(testEnum);
    s1.setmArray(testArray);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmArray(testArray);
    s1.setmStruct(testStruct);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmStruct(testStruct);
    s1.setmU32Arr(testU32Arr);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmU32Arr(testU32Arr);
    s1.setmStructArr(testStructArr);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmStructArr(testStructArr);

    ASSERT_TRUE(s1 == s2);
    ASSERT_FALSE(s1 != s2);
}

// Test struct getter and setter functions
TEST_F(NonPrimitiveStructTest, GetterSetterFunctions) {
    NonPrimitive s1, s2;

    // Set all members
    s1.set(testString, testEnum, testArray, 
           testStruct, testU32Arr, testStructArr);
    assertStructMembers(s1);

    // Set individual members
    s2.setmString(testString);
    ASSERT_EQ(s2.getmString(), testString);

    s2.setmEnum(testEnum);
    ASSERT_EQ(s2.getmEnum(), testEnum);

    s2.setmArray(testArray);
    ASSERT_EQ(s2.getmArray(), testArray);

    s2.setmStruct(testStruct);
    ASSERT_EQ(s2.getmStruct(), testStruct);

    s2.setmU32Arr(testU32Arr);
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s2.getmU32Arr()[i], testU32Arr[i]);
    }

    s2.setmStructArr(testStructArr);
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(s2.getmStructArr()[i], testStructArr[i]);
    }

    // Check non-const getter
    s2.getmStruct().setmU32(testU32Arr[0]);
    ASSERT_EQ(s2.getmStruct().getmU32(), testU32Arr[0]);
}

// Test struct serialization and deserialization
TEST_F(NonPrimitiveStructTest, Serialization) {
    NonPrimitive s(testString, testEnum, testArray, 
                   testStruct, testU32Arr, testStructArr);
    NonPrimitive sCopy;

    U32 stringSerializedSize = testString.length() + sizeof(FwBuffSizeType);
    U32 serializedSize = NonPrimitive::SERIALIZED_SIZE
                         - NonPrimitive::StringSize80::SERIALIZED_SIZE
                         + stringSerializedSize;
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
    assertUnsuccessfulSerialization(s, stringSerializedSize 
            + StructEnum::SERIALIZED_SIZE - 1);
    assertUnsuccessfulSerialization(s, stringSerializedSize 
            + StructEnum::SERIALIZED_SIZE + StructArray::SERIALIZED_SIZE - 1);
    assertUnsuccessfulSerialization(s, stringSerializedSize 
            + StructEnum::SERIALIZED_SIZE + StructArray::SERIALIZED_SIZE 
            + Primitive::SERIALIZED_SIZE - 1);
    assertUnsuccessfulSerialization(s, stringSerializedSize 
            + StructEnum::SERIALIZED_SIZE + StructArray::SERIALIZED_SIZE 
            + Primitive::SERIALIZED_SIZE + (3 * sizeof(U32)) - 1);
    assertUnsuccessfulSerialization(s, serializedSize - 1);
}

// Test struct toString() and ostream operator functions
TEST_F(NonPrimitiveStructTest, ToString) {
    NonPrimitive s(testString, testEnum, testArray, 
                   testStruct, testU32Arr, testStructArr);
    std::stringstream buf1, buf2;

    buf1 << s;

    buf2 << "( "
         << "mString = " << testString << ", "
         << "mEnum = " << testEnum << ", "
         << "mArray = " << testArray << ", "        
         << "mStruct = " << testStruct << ", "        
         << "mU32Arr = [ " 
         << testU32Arr[0] << ", "
         << testU32Arr[1] << ", "
         << testU32Arr[2] << " ], "
         << "mStructArr = [ " 
         << testStructArr[0] << ", "
         << testStructArr[1] << ", "
         << testStructArr[2] << " ] "
         << " )";

    // Truncate string output
    char buf2Str[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE]; 
    Fw::StringUtils::string_copy(buf2Str, buf2.str().c_str(), 
                                 FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE);

    ASSERT_STREQ(buf1.str().c_str(), buf2Str);
}
