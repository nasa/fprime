#include "FppTest/array/StringArrayAc.hpp"
#include "FppTest/utils/Utils.hpp"

#include "Fw/Types/StringUtils.hpp"
#include "Fw/Types/SerialBuffer.hpp"

#include "gtest/gtest.h"

class StringArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        serializedSize = 0;

        char buf[80];
        for (U32 i = 0; i < String::SIZE; i++) {
            FppTest::Utils::setString(buf, sizeof(buf));
            testVals[i] = buf;
            serializedSize += Fw::StringUtils::string_length(buf, sizeof(buf));
        }

        serializedSize += String::SIZE * sizeof(FwBuffSizeType);
    }

    String::StringSize80 testVals[String::SIZE];
    U32 serializedSize; // Length of serial buffer after array is serialized
};

TEST_F(StringArrayTest, Default) {
    String a;

    // Constants
    ASSERT_EQ(String::SIZE, 3);
    ASSERT_EQ(
        String::SERIALIZED_SIZE,
        String::SIZE * String::StringSize80::SERIALIZED_SIZE
    );

    // Default constructor
    for (U32 i = 0; i < String::SIZE; i++) {
        ASSERT_EQ(a[i], "");
    }
}

TEST_F(StringArrayTest, Constructors) {
    // Array constructor
    String a1(testVals);
    for (U32 i = 0; i < String::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[i]);
    }

    // Single element constructor
    String a2(testVals[0]);
    for (U32 i = 0; i < String::SIZE; i++) {
        ASSERT_EQ(a2[i], testVals[0]);
    }

    // Multiple element constructor
    String a3(testVals[0], testVals[1], testVals[2]);
    for (U32 i = 0; i < String::SIZE; i++) {
        ASSERT_EQ(a3[i], testVals[i]);
    }

    // Copy constructor
    String a4(a1);
    for (U32 i = 0; i < String::SIZE; i++) {
        ASSERT_EQ(a4[i], a1[i]);
    }
}

TEST_F(StringArrayTest, SubscriptOp) {
    String a;

    for (U32 i = 0; i < String::SIZE; i++) {
        a[i] = testVals[0];
        ASSERT_EQ(a[i], testVals[0]);
    }
}

TEST_F(StringArrayTest, AssignmentOp) {
    String a1, a2;

    // Array assignment
    a1 = testVals;
    for (U32 i = 0; i < String::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[i]);
    }

    // Copy assignment
    a1 = a2;
    for (U32 i = 0; i < String::SIZE; i++) {
        ASSERT_EQ(a2[i], a1[i]);
    }

    // Single element assignment
    a1 = testVals[0];
    for (U32 i = 0; i < String::SIZE; i++) {
        ASSERT_EQ(a1[i], testVals[0]);
    }
}

TEST_F(StringArrayTest, EqualityOp) {
    String a1, a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);

    a2 = testVals;

    ASSERT_FALSE(a1 == a2);
    ASSERT_TRUE(a1 != a2);

    a1 = a2;

    ASSERT_TRUE(a1 == a2);
    ASSERT_FALSE(a1 != a2);
}

TEST_F(StringArrayTest, Serialization) {
    String a(testVals);
    String aCopy = a;

    Fw::SerializeStatus status;
    U8 data[String::SERIALIZED_SIZE];
    Fw::SerialBuffer buf(data, sizeof(data));

    // Serialize
    status = buf.serialize(a);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buf.getBuffLength(), serializedSize);

    // Deserialize
    status = buf.deserialize(aCopy);

    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(a, aCopy);
}
