// ======================================================================
// \title  PrimitiveTest
// \author R. Bocchino
// \brief  cpp file for primitive struct tests
// ======================================================================

#include <sstream>

#include "FppTest/struct/C_PrimitiveSerializableAc.hpp"
#include "FppTest/struct/PrimitiveSerializableAc.hpp"
#include "FppTest/struct/SM_SMPrimitiveAliasAc.hpp"
#include "FppTest/utils/Utils.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/StringUtils.hpp"
#include "STest/Pick/Pick.hpp"
#include "gtest/gtest.h"

namespace FppTest {

namespace Struct {

template <typename T>
class PrimitiveTest : public ::testing::Test {
  protected:
    void SetUp() override {
        testBool = true;
        testU32 = FppTest::Utils::getNonzeroU32();
        testI16 = static_cast<I16>(FppTest::Utils::getNonzeroU32());
        testF64 = static_cast<F64>(FppTest::Utils::getNonzeroU32());
    }

    void assertStructMembers(const T& s) {
        ASSERT_EQ(s.get_mBool(), testBool);
        ASSERT_EQ(s.get_mU32(), testU32);
        ASSERT_EQ(s.get_mI16(), testI16);
        ASSERT_EQ(s.get_mF64(), testF64);
    }

    void assertUnsuccessfulSerialization(T& s, U32 bufSize) {
        // Avoid creating an array of size zero
        U8 data[bufSize + 1];
        Fw::SerialBuffer buf(data, bufSize);
        Fw::SerializeStatus status;

        // Serialize
        status = buf.serializeFrom(s);
        ASSERT_NE(status, Fw::FW_SERIALIZE_OK);

        // Deserialize
        status = buf.deserializeTo(s);
        ASSERT_NE(status, Fw::FW_SERIALIZE_OK);
    }

    bool testBool;
    U32 testU32;
    I16 testI16;
    F64 testF64;
};

TYPED_TEST_SUITE_P(PrimitiveTest);

// Test struct constants and default constructor
TYPED_TEST_P(PrimitiveTest, Default) {
    TypeParam s;

    // Constants
    ASSERT_EQ(TypeParam::SERIALIZED_SIZE, sizeof(U8) + sizeof(U32) + sizeof(I16) + sizeof(F64));

    // Default constructor
    ASSERT_EQ(s.get_mBool(), false);
    ASSERT_EQ(s.get_mU32(), 0);
    ASSERT_EQ(s.get_mI16(), 0);
    ASSERT_EQ(s.get_mF64(), 0.0);
}

// Test struct constructors
TYPED_TEST_P(PrimitiveTest, Constructors) {
    // Member constructor
    TypeParam s1(this->testBool, this->testU32, this->testI16, this->testF64);
    this->assertStructMembers(s1);

    // Copy constructor
    TypeParam s2(s1);
    this->assertStructMembers(s2);
}

// Test struct assignment operator
TYPED_TEST_P(PrimitiveTest, AssignmentOp) {
    TypeParam s1;
    TypeParam s2(this->testBool, this->testU32, this->testI16, this->testF64);

    // Copy assignment
    s1 = s2;
    this->assertStructMembers(s1);

    TypeParam& s1Ref = s1;
    s1 = s1Ref;
    ASSERT_EQ(&s1, &s1Ref);
}

// Test struct equality and inequality operators
TYPED_TEST_P(PrimitiveTest, EqualityOp) {
    TypeParam s1, s2;

    ASSERT_TRUE(s1 == s2);
    ASSERT_FALSE(s1 != s2);

    s1.set_mBool(this->testBool);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mBool(this->testBool);
    s1.set_mU32(this->testU32);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mU32(this->testU32);
    s1.set_mI16(this->testI16);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mI16(this->testI16);
    s1.set_mF64(this->testF64);

    ASSERT_FALSE(s1 == s2);
    ASSERT_TRUE(s1 != s2);

    s2.set_mF64(this->testF64);

    ASSERT_TRUE(s1 == s2);
    ASSERT_FALSE(s1 != s2);
}

// Test struct getter and setter functions
TYPED_TEST_P(PrimitiveTest, GetterSetterFunctions) {
    TypeParam s1, s2;

    // Set all members
    s1.set(this->testBool, this->testU32, this->testI16, this->testF64);
    this->assertStructMembers(s1);

    // Set individual members
    s2.set_mBool(this->testBool);
    ASSERT_EQ(s2.get_mBool(), this->testBool);

    s2.set_mU32(this->testU32);
    ASSERT_EQ(s2.get_mU32(), this->testU32);

    s2.set_mI16(this->testI16);
    ASSERT_EQ(s2.get_mI16(), this->testI16);

    s2.set_mF64(this->testF64);
    ASSERT_EQ(s2.get_mF64(), this->testF64);
}

// Test struct serialization and deserialization
TYPED_TEST_P(PrimitiveTest, Serialization) {
    TypeParam s(this->testBool, this->testU32, this->testI16, this->testF64);
    TypeParam sCopy;

    Fw::SerializeStatus status;

    // Test successful serialization
    U8 data[TypeParam::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize
    status = buf.serializeFrom(s);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getSize(), TypeParam::SERIALIZED_SIZE);

    // Deserialize
    status = buf.deserializeTo(sCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(s, sCopy);

    // Test unsuccessful serialization
    this->assertUnsuccessfulSerialization(s, sizeof(U8) - 1);
    this->assertUnsuccessfulSerialization(s, sizeof(U8) + sizeof(U32) - 1);
    this->assertUnsuccessfulSerialization(s, sizeof(U8) + sizeof(U32) + sizeof(I16) - 1);
    this->assertUnsuccessfulSerialization(s, TypeParam::SERIALIZED_SIZE - 1);
}

// Test serialized size
TYPED_TEST_P(PrimitiveTest, SerializedSize) {
    TypeParam s;
    ASSERT_EQ(s.serializedSize(), sizeof(U8) + sizeof(U32) + sizeof(I16) + sizeof(F64));
}

// Test struct toString() and ostream operator functions
TYPED_TEST_P(PrimitiveTest, ToString) {
    TypeParam s(this->testBool, this->testU32, this->testI16, this->testF64);
    std::stringstream buf1, buf2;

    buf1 << s;

    buf2 << "( "
         << "mBool = " << this->testBool << ", "
         << "mU32 = " << this->testU32 << ", "
         << "mI16 = " << this->testI16 << ", "
         << "mF64 = " << std::fixed << this->testF64 << " )";

    ASSERT_STREQ(buf1.str().c_str(), buf2.str().c_str());
}

REGISTER_TYPED_TEST_SUITE_P(PrimitiveTest,
                            Default,
                            Constructors,
                            AssignmentOp,
                            EqualityOp,
                            GetterSetterFunctions,
                            Serialization,
                            SerializedSize,
                            ToString);

using PrimitiveTestImplementations = ::testing::Types<C_Primitive, SM_SMPrimitive>;
INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, PrimitiveTest, PrimitiveTestImplementations);

}  // namespace Struct

}  // namespace FppTest
