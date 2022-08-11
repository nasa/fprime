#include "FppTest/array/EnumArrayAc.hpp"
#include "FppTest/array/StringArrayAc.hpp"
#include "FppTest/array/StructArrayAc.hpp"
#include "FppTest/array/Uint32ArrayArrayAc.hpp"
#include "FppTest/utils/Utils.hpp"

#include "Fw/Types/SerialBuffer.hpp"

#include "STest/Pick/Pick.hpp"
#include "gtest/gtest.h"

#include <sstream>

// Set default values for an array type
template <typename ArrayType>
void setDefaultVals(typename ArrayType::ElementType (&a)[ArrayType::SIZE]) {}

template <>
void setDefaultVals<Enum>(E (&a)[Enum::SIZE]) {
    a[0] = E::A;
    a[1] = E::B;
    a[2] = E::C;
}

// Set test values for an array type
template <typename ArrayType>
void setTestVals(typename ArrayType::ElementType (&a)[ArrayType::SIZE]);
        
template<>
void setTestVals<Enum>(E (&a)[Enum::SIZE]) {
    for (int i = 0; i < Enum::SIZE; i++) {
        a[i] = static_cast<E::T>(STest::Pick::startLength(
            E::T::A,
            E::NUM_CONSTANTS
        ));
    }
}

template<>
void setTestVals<String>(String::StringSize80 (&a)[String::SIZE]) {
    char buf[80];
    for (U32 i = 0; i < String::SIZE; i++) {
        FppTest::Utils::setString(buf, sizeof(buf));
        a[i] = buf;
    }
}

template<>
void setTestVals<Struct>(S (&a)[Struct::SIZE]) {
    U32 b[3];
    for (int i = 0; i < Struct::SIZE; i++) {
        for (int j = 0; j < 3; j++) {
            b[j] = FppTest::Utils::getU32();
        }
        a[i].set(FppTest::Utils::getU32(), b);
    }
}

template<>
void setTestVals<Uint32Array>(Uint32 (&a)[Uint32Array::SIZE]) {
    Uint32 b;
    for (int i = 0; i < Uint32Array::SIZE; i++) {
        for (int j = 0; j < Uint32::SIZE; j++) {
            b[j] = FppTest::Utils::getU32();
        }
        a[i] = b;
    }
}

// Get the serialized size of an array
template <typename ArrayType>
U32 getSerializedSize(typename ArrayType::ElementType (&a)[ArrayType::SIZE]) {
    return ArrayType::SERIALIZED_SIZE;
}

template <>
U32 getSerializedSize<String>(String::StringSize80 (&a)[String::SIZE]) {
    U32 serializedSize = 0;

    for (int i = 0; i < String::SIZE; i++) {
        serializedSize += a[i].length() + sizeof(FwBuffSizeType);   
    }

    return serializedSize;
}

// Test an array class
template <typename ArrayType>
class ArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        setDefaultVals<ArrayType>(defaultVals);

        setTestVals<ArrayType>(testVals);
        while (valsAreEqual()) {
            setTestVals<ArrayType>(testVals);
        }
    };

    bool valsAreEqual() {
        for (int i = 0; i < ArrayType::SIZE; i++) {
            if (defaultVals[i] != testVals[i]) {
                return false;
            }
        }

        return true;
    }

    typename ArrayType::ElementType defaultVals[ArrayType::SIZE];
    typename ArrayType::ElementType testVals[ArrayType::SIZE];
};

// Specify type parameters for this test suite
using ArrayTypes = ::testing::Types<
    Enum,
    String,
    Struct,
    Uint32Array
>;
TYPED_TEST_SUITE(ArrayTest, ArrayTypes);

// Test array constants and default constructor
TYPED_TEST(ArrayTest, Default) {
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
TYPED_TEST(ArrayTest, Constructors) {
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
    TypeParam a3(this->testVals[0], this->testVals[1], this->testVals[2]);
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
TYPED_TEST(ArrayTest, SubscriptOp) {
    TypeParam a;

    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        a[i] = this->testVals[0];
        ASSERT_EQ(a[i], this->testVals[0]);
    }
}

// Test array assignment operator
TYPED_TEST(ArrayTest, AssignmentOp) {
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
TYPED_TEST(ArrayTest, EqualityOp) {
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
TYPED_TEST(ArrayTest, Serialization) {
    TypeParam a(this->testVals);

    U32 serializedSize = getSerializedSize<TypeParam>(this->testVals);
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

// Test array toString() and ostream operator functions
TYPED_TEST(ArrayTest, ToString) {
    TypeParam a(this->testVals);
    std::stringstream buf1, buf2;

    buf1 << a;

    buf2 << "[ ";
    for (int i = 0; i < TypeParam::SIZE; i++) {
        buf2 << this->testVals[i] << " ";    
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}
