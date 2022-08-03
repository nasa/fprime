//
// Created by mstarch on 11/13/20.
//
#include "Fw/Buffer/Buffer.hpp"
#include <FpConfig.hpp>
#include <gtest/gtest.h>


void test_basic() {
    U8 data[100];
    U8 faux[100];
    Fw::Buffer buffer;
    // Check basic guarantees
    ASSERT_EQ(buffer.m_context, Fw::Buffer::NO_CONTEXT);
    buffer.setData(data);
    buffer.setSize(sizeof(data));
    buffer.setContext(1234);
    ASSERT_EQ(buffer.getData(), data);
    ASSERT_EQ(buffer.getSize(), sizeof(data));
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
    Fw::Buffer testBuffer(nullptr,0);
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
    buffer_new.setSize(0);
    buffer_new.setData(faux);
    buffer_new.setContext(22222);
    buffer_assignment1.setSize(0);
    buffer_assignment1.setData(faux);
    buffer_assignment1.setContext(22222);
    buffer_assignment2.setSize(0);
    buffer_assignment2.setData(faux);
    buffer_assignment2.setContext(22222);

    ASSERT_EQ(buffer.getData(), data);
    ASSERT_EQ(buffer.getSize(), sizeof(data));
    ASSERT_EQ(buffer.getContext(), 1234);
}

void test_representations() {
    U8 data[100];
    Fw::Buffer buffer;
    buffer.setData(data);
    buffer.setSize(sizeof(data));
    buffer.setContext(1234);

    // Test serialization and that it stops before overflowing
    Fw::SerializeBufferBase& sbb = buffer.getSerializeRepr();
    sbb.resetSer();
    for (U32 i = 0; i < sizeof(data)/4; i++) {
        ASSERT_EQ(sbb.serialize(i), Fw::FW_SERIALIZE_OK);
    }
    Fw::SerializeStatus stat = sbb.serialize(100);
    ASSERT_NE(stat, Fw::FW_SERIALIZE_OK);
    // And that another call to repr resets it
    sbb = buffer.getSerializeRepr();
    sbb.resetSer();
    ASSERT_EQ(sbb.serialize(0), Fw::FW_SERIALIZE_OK);

    // Now deserialize all the things
    U32 out;
    sbb = buffer.getSerializeRepr();
    sbb.setBuffLen(buffer.getSize());
    for (U32 i = 0; i < sizeof(data)/4; i++) {
        ASSERT_EQ(sbb.deserialize(out), Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(i, out);
    }
    ASSERT_NE(sbb.deserialize(out), Fw::FW_SERIALIZE_OK);
    sbb = buffer.getSerializeRepr();
    sbb.setBuffLen(buffer.getSize());
    ASSERT_EQ(sbb.deserialize(out), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(0, out);
}

void test_serialization() {
    U8 data[100];
    U8 wire[100];

    Fw::Buffer buffer;
    buffer.setData(data);
    buffer.setSize(sizeof(data));
    buffer.setContext(1234);

    Fw::ExternalSerializeBuffer externalSerializeBuffer(wire, sizeof(wire));
    externalSerializeBuffer.serialize(buffer);
    ASSERT_LT(externalSerializeBuffer.m_serLoc, sizeof(data));

    Fw::Buffer buffer_new;
    externalSerializeBuffer.deserialize(buffer_new);
    ASSERT_EQ(buffer_new, buffer);

    // Make sure internal ExternalSerializeBuffer is reinitialized
    ASSERT_EQ(buffer_new.m_serialize_repr.m_buff,data);
    ASSERT_EQ(buffer_new.m_serialize_repr.m_buffSize,sizeof(data));
}


TEST(Nominal, BasicBuffer) {
    test_basic();
}

TEST(Nominal, Representations) {
    test_representations();
}

TEST(Nominal, Serialization) {
    test_serialization();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
