// ======================================================================
// \title  PrimitiveStructTest.cpp
// \author T. Chieu
// \brief  cpp file for PrimitiveStructTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/struct/PrimitiveSerializableAc.hpp"
#include "FppTest/utils/Utils.hpp"

#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "STest/Pick/Pick.hpp"

#include "gtest/gtest.h"

#include <sstream>

// Test Primitive struct class
class PrimitiveStructTest : public ::testing::Test {
protected:
    void SetUp() override {
        testBool = true;
        testU32 = FppTest::Utils::getU32();
        testI16 = static_cast<I16>(FppTest::Utils::getU32());
        testF64 = static_cast<F64>(FppTest::Utils::getU32());
    }

    void assertStructMembers(const Primitive& s) {
        ASSERT_EQ(s.getmBool(), testBool);
        ASSERT_EQ(s.getmU32(), testU32);
        ASSERT_EQ(s.getmI16(), testI16);
        ASSERT_EQ(s.getmF64(), testF64);
    }

    void assertUnsuccessfulSerialization(Primitive& s, U32 bufSize) {
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

    bool testBool;
    U32 testU32;
    I16 testI16;
    F64 testF64;
};

// Test struct constants and default constructor
TEST_F(PrimitiveStructTest, Default) {
    Primitive s;

    // Constants
    ASSERT_EQ(
        Primitive::SERIALIZED_SIZE,
        sizeof(U8) 
            + sizeof(U32) 
            + sizeof(I16) 
            + sizeof(F64) 
    );

    // Default constructor
    ASSERT_EQ(s.getmBool(), false);
    ASSERT_EQ(s.getmU32(), 0);
    ASSERT_EQ(s.getmI16(), 0);
    ASSERT_EQ(s.getmF64(), 0.0);
}

// Test struct constructors
TEST_F(PrimitiveStructTest, Constructors) {
    // Member constructor
    Primitive s1(testBool, testU32, testI16, testF64);
    assertStructMembers(s1);

    // Copy constructor
    Primitive s2(s1);
    assertStructMembers(s2);
}

// Test struct assignment operator
TEST_F(PrimitiveStructTest, AssignmentOp) {
    Primitive s1;
    Primitive s2(testBool, testU32, testI16, testF64);

    // Copy assignment
    s1 = s2;
    assertStructMembers(s1);

    Primitive& s1Ref = s1;
    s1 = s1Ref;
    ASSERT_EQ(&s1, &s1Ref);
}

// Test struct equality and inequality operators
TEST_F(PrimitiveStructTest, EqualityOp) {
    Primitive s1, s2;

    ASSERT_TRUE(s1 == s2);
    ASSERT_FALSE(s1 != s2);

    s1.setmBool(testBool);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmBool(testBool);
    s1.setmU32(testU32);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmU32(testU32);
    s1.setmI16(testI16);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmI16(testI16);
    s1.setmF64(testF64);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.setmF64(testF64);

    ASSERT_TRUE(s1 == s2);
    ASSERT_FALSE(s1 != s2);
}

// Test struct getter and setter functions
TEST_F(PrimitiveStructTest, GetterSetterFunctions) {
    Primitive s1, s2;

    // Set all members
    s1.set(testBool, testU32, testI16, testF64);
    assertStructMembers(s1);

    // Set individual members
    s2.setmBool(testBool);
    ASSERT_EQ(s2.getmBool(), testBool);

    s2.setmU32(testU32);
    ASSERT_EQ(s2.getmU32(), testU32);

    s2.setmI16(testI16);
    ASSERT_EQ(s2.getmI16(), testI16);

    s2.setmF64(testF64);
    ASSERT_EQ(s2.getmF64(), testF64);
}

// Test struct serialization and deserialization
TEST_F(PrimitiveStructTest, Serialization) {
    Primitive s(testBool, testU32, testI16, testF64);
    Primitive sCopy;

    Fw::SerializeStatus status;

    // Test successful serialization
    U8 data[Primitive::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize
    status = buf.serialize(s);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getBuffLength(), Primitive::SERIALIZED_SIZE);

    // Deserialize
    status = buf.deserialize(sCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(s, sCopy);

    // Test unsuccessful serialization
    assertUnsuccessfulSerialization(s, sizeof(U8) - 1);
    assertUnsuccessfulSerialization(s, sizeof(U8) + sizeof(U32) - 1);
    assertUnsuccessfulSerialization(s, sizeof(U8) + sizeof(U32) 
                                       + sizeof(I16) - 1);
    assertUnsuccessfulSerialization(s, Primitive::SERIALIZED_SIZE - 1);
}

// Test struct toString() and ostream operator functions
TEST_F(PrimitiveStructTest, ToString) {
    Primitive s(testBool, testU32, testI16, testF64);
    std::stringstream buf1, buf2;

    buf1 << s;

    buf2 << "( "
         << "mBool = " << testBool << ", "       
         << "mU32 = " << testU32 << ", "
         << "mI16 = " << testI16 << ", "        
         << "mF64 = " << std::fixed << testF64        
         << " )";

    // Truncate string output
    char buf2Str[FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE]; 
    Fw::StringUtils::string_copy(buf2Str, buf2.str().c_str(), 
                                 FW_SERIALIZABLE_TO_STRING_BUFFER_SIZE);

    ASSERT_STREQ(buf1.str().c_str(), buf2Str);
}
