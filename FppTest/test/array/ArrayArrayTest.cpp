#include "FppTest/test/array/Uint32ArrayArrayAc.hpp"
#include "FppTest/test/utils/utils.hpp"

#include "Fw/Types/SerialBuffer.hpp"

#include "gtest/gtest.h"

class Uint32ArrayArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        Uint32 a;
        for (int i = 0; i < Uint32Array::SIZE; i++) {
            for (int j = 0; j < Uint32::SIZE; j++) {
                a[j] = getU32();
            }
            testVals[i] = a;
        }
    }

    Uint32 testVals[Uint32Array::SIZE];
};

TEST_F(Uint32ArrayArrayTest, Default) {
    Uint32Array a;
    Uint32 defaultVal;

    // Constants
    ASSERT_EQ(Uint32Array::SIZE, 3);
    ASSERT_EQ(
        Uint32Array::SERIALIZED_SIZE,
        Uint32Array::SIZE * Uint32::SIZE * sizeof(U32)
    );

    // Default constructor
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        ASSERT_EQ(a[i], defaultVal);
    }
}

TEST_F(Uint32ArrayArrayTest, Constructors) {
    // Array constructor
    Uint32Array a1(testVals);
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[i]);
    }

    // Single element constructor
    Uint32Array a2(testVals[0]);
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        ASSERT_EQ(a2[i], testVals[0]);
    }

    // Multiple element constructor
    Uint32Array a3(testVals[0], testVals[1], testVals[2]);
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        ASSERT_EQ(a3[i], testVals[i]);
    }

    // Copy constructor
    Uint32Array a4(a1);
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        ASSERT_EQ(a4[i], a1[i]);
    }
}

TEST_F(Uint32ArrayArrayTest, SubscriptOp) {
    Uint32Array a;

    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        a[i] = testVals[0];
        ASSERT_EQ(a[i], testVals[0]);
    }
}

TEST_F(Uint32ArrayArrayTest, AssignmentOp) {
    Uint32Array a1, a2;

    // Array assignment
    a1 = testVals;
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[i]);
    }

    // Copy assignment
    a1 = a2;
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        ASSERT_EQ(a2[i], a1[i]);
    }

    // Single element assignment
    a1 = testVals[0];
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[0]);
    }
}

TEST_F(Uint32ArrayArrayTest, EqualityOp) {
    Uint32Array a1, a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);

    a2 = testVals;

    ASSERT_FALSE(a1 == a2);
    ASSERT_TRUE(a1 != a2);

    a1 = a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);
}

TEST_F(Uint32ArrayArrayTest, Serialization) {
    Uint32Array a(testVals);
    Uint32Array aCopy = a;

    Fw::SerializeStatus status;
    U8 data[Uint32Array::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize
    status = buf.serialize(a);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getBuffLength(), Uint32Array::SERIALIZED_SIZE);

    // Deserialize
    status = buf.deserialize(aCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(a, aCopy);
}
