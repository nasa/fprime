#include "FppTest/test/enum/ImplicitEnumAc.hpp"
#include "FppTest/test/enum/ExplicitEnumAc.hpp"
#include "FppTest/test/enum/DefaultEnumAc.hpp"
#include "FppTest/test/enum/IntervalEnumAc.hpp"
#include "FppTest/test/enum/SerializeTypeU8EnumAc.hpp"
#include "FppTest/test/enum/SerializeTypeU64EnumAc.hpp"

#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/Assert.hpp"

#include "Fw/Test/UnitTestAssert.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

#include <sstream>
#include <cstring>
#include <limits>

// Returns a valid value for Implicit enum
Implicit::T getValidValue() {
    return static_cast<Implicit::T>(STest::Pick::startLength(
        Implicit::T::A, 
        Implicit::NUM_CONSTANTS
    ));
}

// Returns an invalid value for Implicit enum
Implicit::T getInvalidValue() {
    return static_cast<Implicit::T>(STest::Pick::startLength(
        Implicit::T::C + 1, 
        std::numeric_limits<I32>::max() - Implicit::NUM_CONSTANTS
    ));
}

TEST(EnumTest, Initialization) {
    Implicit implicitEnum;
    Explicit explicitEnum;
    Default defaultEnum;
    Interval intervalEnum;
    SerializeTypeU8 typeU8Enum;
    SerializeTypeU64 typeU64Enum;

    Implicit::T validVal = getValidValue();

    // SERIALIZED_SIZE
    ASSERT_EQ(Implicit::SERIALIZED_SIZE, sizeof(I32));
    ASSERT_EQ(Explicit::SERIALIZED_SIZE, sizeof(I32));
    ASSERT_EQ(Default::SERIALIZED_SIZE, sizeof(I32));
    ASSERT_EQ(Interval::SERIALIZED_SIZE, sizeof(I32));
    ASSERT_EQ(SerializeTypeU8::SERIALIZED_SIZE, sizeof(U8));
    ASSERT_EQ(SerializeTypeU64::SERIALIZED_SIZE, sizeof(U64));

    // Default constructor
    ASSERT_EQ(implicitEnum.e, Implicit::T::A);
    ASSERT_EQ(explicitEnum.e, Explicit::T::A);
    ASSERT_EQ(defaultEnum.e, Default::T::D);
    ASSERT_EQ(intervalEnum.e, Interval::T::A);
    ASSERT_EQ(typeU8Enum.e, SerializeTypeU8::T::A);
    ASSERT_EQ(typeU64Enum.e, SerializeTypeU64::T::A);

    // Raw enum value constructor
    Implicit implicitEnum2(validVal);
    ASSERT_EQ(implicitEnum2.e, validVal);

    // Copy constructor
    Implicit implicitEnum3(implicitEnum2);
    ASSERT_EQ(implicitEnum3.e, validVal);
}

TEST(EnumTest, AssignmentOp) {
    Implicit implicitEnum1;
    Implicit implicitEnum2;

    Implicit::T validVal = getValidValue();

    // Raw enum value assignment
    implicitEnum1 = validVal;
    ASSERT_EQ(implicitEnum1.e, validVal);
    
    // Object assignment
    implicitEnum2 = implicitEnum1;
    ASSERT_EQ(implicitEnum2.e, validVal);
}

TEST(EnumTest, EqualityOp) {
    // Initialize two distinct valid values
    Implicit::T validVal1 = getValidValue();
    Implicit::T validVal2 = getValidValue();
    while (validVal1 == validVal2) {
        validVal2 = getValidValue();
    }

    Implicit implicitEnum1;
    Implicit implicitEnum2;
    Implicit implicitEnum3(validVal1);
    Implicit implicitEnum4(validVal2);

    // operator==
    ASSERT_TRUE(implicitEnum3 == validVal1);
    ASSERT_TRUE(implicitEnum4 == validVal2);
    ASSERT_FALSE(implicitEnum3 == validVal2);
    ASSERT_FALSE(implicitEnum4 == validVal1);

    ASSERT_TRUE(implicitEnum1 == implicitEnum2);
    ASSERT_FALSE(implicitEnum3 == implicitEnum4);

    // operator!=
    ASSERT_TRUE(implicitEnum3 != validVal2);
    ASSERT_TRUE(implicitEnum4 != validVal1);
    ASSERT_FALSE(implicitEnum3 != validVal1);
    ASSERT_FALSE(implicitEnum4 != validVal2);

    ASSERT_TRUE(implicitEnum3 != implicitEnum4);
    ASSERT_FALSE(implicitEnum1 != implicitEnum2);
}

TEST(EnumTest, StringFunctions) {
    Implicit implicitEnum;

    Implicit::T validVal = getValidValue();
    Implicit::T invalidVal = getInvalidValue();

    std::stringstream buf1;
    std::stringstream buf2;

    implicitEnum = validVal;
    buf1 << implicitEnum;
    switch (validVal) {
        case Implicit::T::A:
            ASSERT_STREQ(
                buf1.str().c_str(), 
                "A (0)"
            );
            break;
        case Implicit::T::B:
            ASSERT_STREQ(
                buf1.str().c_str(), 
                "B (1)"
            );
            break;
        case Implicit::T::C:
            ASSERT_STREQ(
                buf1.str().c_str(), 
                "C (2)"
            );
            break;
    }

    implicitEnum = invalidVal;
    buf2 << implicitEnum;
    std::string str = "[invalid] (" + std::to_string(invalidVal) + ")";
    ASSERT_STREQ(
        buf2.str().c_str(),
        str.c_str()
    );
}

TEST(EnumTest, IsValidFunction) {
    Implicit validEnum = getValidValue();
    Implicit invalidEnum = getInvalidValue();

    ASSERT_TRUE(validEnum.isValid());
    ASSERT_FALSE(invalidEnum.isValid());

    // Test boundary values
    Interval intervalEnumN1 = static_cast<Interval::T>(-1);
    Interval intervalEnum0 = static_cast<Interval::T>(0);
    Interval intervalEnum1 = static_cast<Interval::T>(1);
    Interval intervalEnum2 = static_cast<Interval::T>(2);
    Interval intervalEnum3 = static_cast<Interval::T>(3);
    Interval intervalEnum5 = static_cast<Interval::T>(5);
    Interval intervalEnum6 = static_cast<Interval::T>(6);
    Interval intervalEnum10 = static_cast<Interval::T>(10);
    Interval intervalEnum99 = static_cast<Interval::T>(99);
    Interval intervalEnum100 = static_cast<Interval::T>(100);
    Interval intervalEnum101 = static_cast<Interval::T>(101);
    Interval intervalEnum102 = static_cast<Interval::T>(102);

    ASSERT_FALSE(intervalEnumN1.isValid());
    ASSERT_TRUE(intervalEnum0.isValid());
    ASSERT_FALSE(intervalEnum1.isValid());
    ASSERT_FALSE(intervalEnum2.isValid());
    ASSERT_TRUE(intervalEnum3.isValid());
    ASSERT_TRUE(intervalEnum5.isValid());
    ASSERT_FALSE(intervalEnum6.isValid());
    ASSERT_TRUE(intervalEnum10.isValid());
    ASSERT_FALSE(intervalEnum99.isValid());
    ASSERT_TRUE(intervalEnum100.isValid());
    ASSERT_TRUE(intervalEnum101.isValid());
    ASSERT_FALSE(intervalEnum102.isValid());
}

TEST(EnumTest, Serialization) {
    Implicit enumI32;
    SerializeTypeU8 enumU8;
    SerializeTypeU64 enumU64;

    // Copy of enums to test after serialization
    Implicit enumI32Copy = enumI32;
    SerializeTypeU8 enumU8Copy = enumU8;
    SerializeTypeU64 enumU64Copy = enumU64;

    Fw::SerializeStatus status1;
    Fw::SerializeStatus status2;
    Fw::SerializeStatus status3;
    U8 data1[Implicit::SERIALIZED_SIZE];
    U8 data2[SerializeTypeU8::SERIALIZED_SIZE];
    U8 data3[SerializeTypeU64::SERIALIZED_SIZE];
    Fw::SerialBuffer buf1(data1, sizeof(data1));
    Fw::SerialBuffer buf2(data2, sizeof(data2));
    Fw::SerialBuffer buf3(data3, sizeof(data3));

    // Serialize the enums
    status1 = buf1.serialize(enumI32);
    status2 = buf2.serialize(enumU8);
    status3 = buf3.serialize(enumU64);

    ASSERT_EQ(status1, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(status2, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(status3, Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(buf1.getBuffLength(), sizeof(I32));
    ASSERT_EQ(buf2.getBuffLength(), sizeof(U8));
    ASSERT_EQ(buf3.getBuffLength(), sizeof(U64));

    // Deserialize the enums
    status1 = buf1.deserialize(enumI32Copy);
    status2 = buf2.deserialize(enumU8Copy);
    status3 = buf3.deserialize(enumU64Copy);

    ASSERT_EQ(status1, Fw::FW_SERIALIZE_OK);
    // Value of enumU8 is too large to fit in a U8
    ASSERT_EQ(status2, Fw::FW_DESERIALIZE_FORMAT_ERROR);
    ASSERT_EQ(status3, Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(enumI32, enumI32Copy);
    // Value of enumU8 is too large to fit in a U8
    ASSERT_NE(enumU8, enumU8Copy);
    ASSERT_EQ(enumU64, enumU64Copy);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
