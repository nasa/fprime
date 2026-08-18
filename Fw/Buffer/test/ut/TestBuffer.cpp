//
// Created by mstarch on 11/13/20.
//
#include <gtest/gtest.h>
#include <Fw/FPrimeBasicTypes.hpp>
#include "Fw/Buffer/Buffer.hpp"
#include "Fw/Types/test/ut/LinearBufferBaseTester.hpp"

namespace Fw {
class BufferTester {
  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------
    BufferTester() {}

    ~BufferTester() {}

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------
    void test_basic() {
        U8 data[100];
        U8 faux[100];
        Fw::Buffer buffer;
        // Check basic guarantees
        ASSERT_EQ(buffer.m_context, Fw::Buffer::NO_CONTEXT);
        buffer.set(data, sizeof(data));
        buffer.setContext(1234);
        ASSERT_EQ(buffer.getData(), data);
        ASSERT_EQ(buffer.getOriginalData(), data);
        ASSERT_EQ(buffer.getSize(), sizeof(data));
        ASSERT_EQ(buffer.getCapacity(), sizeof(data));
        ASSERT_EQ(buffer.getOffset(), 0);
        ASSERT_EQ(buffer.getContext(), 1234);

        // Test set method is equivalent
        Fw::Buffer buffer_set;
        buffer_set.set(data, sizeof(data), 1234);
        ASSERT_EQ(buffer_set, buffer);

        // Check constructors and assignments
        Fw::Buffer buffer_new(buffer);
        ASSERT_EQ(buffer_new.getData(), data);
        ASSERT_EQ(buffer_new.getSize(), sizeof(data));
        ASSERT_EQ(buffer_new.getContext(), 1234);
        ASSERT_EQ(buffer, buffer_new);

        // Creating empty buffer
        Fw::Buffer testBuffer(nullptr, 0);
        ASSERT_EQ(testBuffer.getData(), nullptr);
        ASSERT_EQ(testBuffer.getSize(), 0);

        // Assignment operator with transitivity
        Fw::Buffer buffer_assignment1, buffer_assignment2;
        ASSERT_NE(buffer_assignment1.getData(), data);
        ASSERT_NE(buffer_assignment1.getSize(), sizeof(data));
        ASSERT_NE(buffer_assignment1.getContext(), 1234);
        ASSERT_NE(buffer_assignment2.getData(), data);
        ASSERT_NE(buffer_assignment2.getSize(), sizeof(data));
        ASSERT_NE(buffer_assignment2.getContext(), 1234);
        buffer_assignment1 = buffer_assignment2 = buffer;
        ASSERT_EQ(buffer_assignment1.getData(), data);
        ASSERT_EQ(buffer_assignment1.getSize(), sizeof(data));
        ASSERT_EQ(buffer_assignment1.getContext(), 1234);
        ASSERT_EQ(buffer_assignment2.getData(), data);
        ASSERT_EQ(buffer_assignment2.getSize(), sizeof(data));
        ASSERT_EQ(buffer_assignment2.getContext(), 1234);

        // Check modifying the copies does not destroy
        buffer_new.set(faux, 0);
        buffer_new.setContext(22222);
        buffer_assignment1.set(faux, 0);
        buffer_assignment1.setContext(22222);
        buffer_assignment2.set(faux, 0);
        buffer_assignment2.setContext(22222);

        ASSERT_EQ(buffer.getData(), data);
        ASSERT_EQ(buffer.getSize(), sizeof(data));
        ASSERT_EQ(buffer.getContext(), 1234);
    }

    void test_advance() {
        U8 data[100];
        Fw::Buffer buffer(data, sizeof(data), 1234);

        // Advance forward: offset grows, size shrinks, end fixed, original recoverable
        buffer.advance(10);
        ASSERT_EQ(buffer.getData(), data + 10);
        ASSERT_EQ(buffer.getOriginalData(), data);
        ASSERT_EQ(buffer.getOffset(), 10);
        ASSERT_EQ(buffer.getSize(), sizeof(data) - 10);
        ASSERT_EQ(buffer.getCapacity(), sizeof(data));

        // Advance backward restores
        buffer.advance(-10);
        ASSERT_EQ(buffer.getData(), data);
        ASSERT_EQ(buffer.getOffset(), 0);
        ASSERT_EQ(buffer.getSize(), sizeof(data));

        // setData within the original allocation updates the offset
        buffer.setData(data + 25);
        ASSERT_EQ(buffer.getData(), data + 25);
        ASSERT_EQ(buffer.getOriginalData(), data);
        ASSERT_EQ(buffer.getOffset(), 25);
        ASSERT_EQ(buffer.getSize(), sizeof(data) - 25);

        // setSize is checked against offset + capacity
        buffer.setSize(sizeof(data) - 25);
        ASSERT_EQ(buffer.getSize(), sizeof(data) - 25);

        // Copies preserve offset and capacity
        Fw::Buffer copy(buffer);
        ASSERT_EQ(copy.getOriginalData(), data);
        ASSERT_EQ(copy.getOffset(), 25);
        ASSERT_EQ(copy.getCapacity(), sizeof(data));
        ASSERT_EQ(copy, buffer);

        // Out-of-bounds operations assert
        U8* unrelated = new U8[100];
        ASSERT_DEATH_IF_SUPPORTED(buffer.setData(unrelated), "");
        delete[] unrelated;
        ASSERT_DEATH_IF_SUPPORTED(buffer.setSize(sizeof(data) - 25 + 1), "");
        ASSERT_DEATH_IF_SUPPORTED(buffer.advance(-26), "");
        ASSERT_DEATH_IF_SUPPORTED(buffer.advance(static_cast<FwSignedSizeType>(sizeof(data))), "");
    }

    void test_representations() {
        U8 data[100];
        Fw::Buffer buffer(data, sizeof(data), 1234);

        // Test serialization and that it stops before overflowing
        auto serializer = buffer.getSerializer();
        for (U32 i = 0; i < sizeof(data) / 4; i++) {
            ASSERT_EQ(serializer.serializeFrom(i), Fw::FW_SERIALIZE_OK);
        }
        Fw::SerializeStatus stat = serializer.serializeFrom(100);
        ASSERT_NE(stat, Fw::FW_SERIALIZE_OK);

        // And that another call to repr resets it
        serializer.resetSer();
        ASSERT_EQ(serializer.serializeFrom(0), Fw::FW_SERIALIZE_OK);

        // Now deserialize all the things
        auto deserializer = buffer.getDeserializer();
        U32 out;
        for (U32 i = 0; i < sizeof(data) / 4; i++) {
            ASSERT_EQ(deserializer.deserializeTo(out), Fw::FW_SERIALIZE_OK);
            ASSERT_EQ(i, out);
        }
        ASSERT_NE(deserializer.deserializeTo(out), Fw::FW_SERIALIZE_OK);
        deserializer.setBuffLen(buffer.getSize());
        ASSERT_EQ(deserializer.deserializeTo(out), Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(0, out);
    }

    void test_serialization() {
        U8 data[100];
        U8 wire[100];

        Fw::Buffer buffer(data, sizeof(data), 1234);
        buffer.advance(7);

        Fw::ExternalSerializeBuffer externalSerializeBuffer(wire, sizeof(wire));
        externalSerializeBuffer.serializeFrom(buffer);
        Fw::LinearBufferBaseTester::verifySerLocLT(externalSerializeBuffer, sizeof(data));

        Fw::Buffer buffer_new;
        externalSerializeBuffer.deserializeTo(buffer_new);
        ASSERT_EQ(buffer_new, buffer);
        ASSERT_EQ(buffer_new.getOriginalData(), data);
        ASSERT_EQ(buffer_new.getData(), data + 7);
        ASSERT_EQ(buffer_new.getOffset(), 7);
        ASSERT_EQ(buffer_new.getCapacity(), sizeof(data));
    }
};
}  // namespace Fw

TEST(Nominal, BasicBuffer) {
    Fw::BufferTester tester;
    tester.test_basic();
}

TEST(Nominal, Advance) {
    Fw::BufferTester tester;
    tester.test_advance();
}

TEST(Nominal, Representations) {
    Fw::BufferTester tester;
    tester.test_representations();
}

TEST(Nominal, Serialization) {
    Fw::BufferTester tester;
    tester.test_serialization();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
