#include "FppTest/array/StructArrayAc.hpp"
#include "FppTest/utils/Utils.hpp"

#include "Fw/Types/SerialBuffer.hpp"

#include "gtest/gtest.h"

class StructArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        U32 a[3];
        for (int i = 0; i < Struct::SIZE; i++) {
            for (int j = 0; j < 3; j++) {
                a[j] = FppTest::Utils::getU32();
            }
            testVals[i].set(FppTest::Utils::getU32(), a);
        }
    }

    S testVals[Struct::SIZE];
};

TEST_F(StructArrayTest, Default) {
    Struct a;
    S defaultVal;

    // Constants
    ASSERT_EQ(Struct::SIZE, 3);
    ASSERT_EQ(
        Struct::SERIALIZED_SIZE,
        Struct::SIZE * S::SERIALIZED_SIZE
    );

    // Default constructor
    for (U32 i = 0; i < Struct::SIZE; i++) {
        ASSERT_EQ(a[i], defaultVal);
    }
}

TEST_F(StructArrayTest, Constructors) {
    // Array constructor
    Struct a1(testVals);
    for (U32 i = 0; i < Struct::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[i]);
    }

    // Single element constructor
    Struct a2(testVals[0]);
    for (U32 i = 0; i < Struct::SIZE; i++) {
        ASSERT_EQ(a2[i], testVals[0]);
    }

    // Multiple element constructor
    Struct a3(testVals[0], testVals[1], testVals[2]);
    for (U32 i = 0; i < Struct::SIZE; i++) {
        ASSERT_EQ(a3[i], testVals[i]);
    }

    // Copy constructor
    Struct a4(a1);
    for (U32 i = 0; i < Struct::SIZE; i++) {
        ASSERT_EQ(a4[i], a1[i]);
    }
}

TEST_F(StructArrayTest, SubscriptOp) {
    Struct a;

    for (U32 i = 0; i < Struct::SIZE; i++) {
        a[i] = testVals[0];
        ASSERT_EQ(a[i], testVals[0]);
    }
}

TEST_F(StructArrayTest, AssignmentOp) {
    Struct a1, a2;

    // Array assignment
    a1 = testVals;
    for (U32 i = 0; i < Struct::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[i]);
    }

    // Copy assignment
    a1 = a2;
    for (U32 i = 0; i < Struct::SIZE; i++) {
        ASSERT_EQ(a2[i], a1[i]);
    }

    // Single element assignment
    a1 = testVals[0];
    for (U32 i = 0; i < Struct::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[0]);
    }
}

TEST_F(StructArrayTest, EqualityOp) {
    Struct a1, a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);

    a2 = testVals;

    ASSERT_FALSE(a1 == a2);
    ASSERT_TRUE(a1 != a2);

    a1 = a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);
}

TEST_F(StructArrayTest, Serialization) {
    Struct a(testVals);
    Struct aCopy = a;

    Fw::SerializeStatus status;
    U8 data[Struct::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize
    status = buf.serialize(a);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getBuffLength(), Struct::SERIALIZED_SIZE);

    // Deserialize
    status = buf.deserialize(aCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(a, aCopy);
}
