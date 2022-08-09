#include "FppTest/array/EnumArrayAc.hpp"

#include "STest/Pick/Pick.hpp"
#include "Fw/Types/SerialBuffer.hpp"

#include "gtest/gtest.h"

class EnumArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (int i = 0; i < Enum::SIZE; i++) {
            testVals[i] = static_cast<E::T>(STest::Pick::startLength(
                E::T::A,
                E::NUM_CONSTANTS
            ));
        }
    }

    E testVals[Enum::SIZE];
};

TEST_F(EnumArrayTest, Default) {
    Enum a;

    // Constants
    ASSERT_EQ(Enum::SIZE, 3);
    ASSERT_EQ(
        Enum::SERIALIZED_SIZE,
        Enum::SIZE * E::SERIALIZED_SIZE
    );

    // Default constructor
    ASSERT_EQ(a[0], E::T::A);
    ASSERT_EQ(a[1], E::T::B);
    ASSERT_EQ(a[2], E::T::C);
}

TEST_F(EnumArrayTest, Constructors) {
    // Array constructor
    Enum a1(testVals);
    for (U32 i = 0; i < Enum::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[i]);
    }

    // Single element constructor
    Enum a2(testVals[0]);
    for (U32 i = 0; i < Enum::SIZE; i++) {
        ASSERT_EQ(a2[i], testVals[0]);
    }

    // Multiple element constructor
    Enum a3(testVals[0], testVals[1], testVals[2]);
    for (U32 i = 0; i < Enum::SIZE; i++) {
        ASSERT_EQ(a3[i], testVals[i]);
    }

    // Copy constructor
    Enum a4(a1);
    for (U32 i = 0; i < Enum::SIZE; i++) {
        ASSERT_EQ(a4[i], a1[i]);
    }
}

TEST_F(EnumArrayTest, SubscriptOp) {
    Enum a;

    for (U32 i = 0; i < Enum::SIZE; i++) {
        a[i] = testVals[0];
        ASSERT_EQ(a[i], testVals[0]);
    }
}

TEST_F(EnumArrayTest, AssignmentOp) {
    Enum a1, a2;

    // Array assignment
    a1 = testVals;
    for (U32 i = 0; i < Enum::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[i]);
    }

    // Copy assignment
    a1 = a2;
    for (U32 i = 0; i < Enum::SIZE; i++) {
        ASSERT_EQ(a2[i], a1[i]);
    }

    // Single element assignment
    a1 = testVals[0];
    for (U32 i = 0; i < Enum::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[0]);
    }
}

TEST_F(EnumArrayTest, EqualityOp) {
    Enum a1, a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);

    a2[0] = E::T::C;
    a2[1] = E::T::B;
    a2[2] = E::T::A;

    ASSERT_FALSE(a1 == a2);
    ASSERT_TRUE(a1 != a2);

    a1 = a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);
}

TEST_F(EnumArrayTest, Serialization) {
    Enum a(testVals);
    Enum aCopy = a;

    Fw::SerializeStatus status;
    U8 data[Enum::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize
    status = buf.serialize(a);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getBuffLength(), Enum::SERIALIZED_SIZE);

    // Deserialize
    status = buf.deserialize(aCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(a, aCopy);
}
