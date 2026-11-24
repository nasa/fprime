#include <gtest/gtest.h>
#include <Fw/FPrimeBasicTypes.hpp>

#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/Serializable.hpp"

namespace ExternalSerializeBufferTest {

using SizeType = Fw::Serializable::SizeType;

constexpr SizeType BUFFER_SIZE = 10;

U8 buffer[BUFFER_SIZE];

void serializeOK(Fw::ExternalSerializeBuffer& esb) {
    const SizeType buffCapacity = esb.getCapacity();
    ASSERT_EQ(esb.getSize(), 0);
    for (SizeType i = 0; i < buffCapacity; i++) {
        const U8 value = static_cast<U8>(i);
        const Fw::SerializeStatus status = esb.serializeFrom(value);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    }
    ASSERT_EQ(esb.getSize(), buffCapacity);
}

void serializeFail(Fw::ExternalSerializeBuffer& esb) {
    ASSERT_EQ(esb.getSize(), esb.getCapacity());
    const Fw::SerializeStatus status = esb.serializeFrom(static_cast<U8>(0));
    ASSERT_EQ(status, Fw::FW_SERIALIZE_NO_ROOM_LEFT);
}

void deserializeOK(Fw::ExternalSerializeBuffer& esb) {
    const SizeType buffCapacity = esb.getCapacity();
    ASSERT_EQ(esb.getDeserializeSizeLeft(), buffCapacity);
    for (SizeType i = 0; i < buffCapacity; i++) {
        U8 value = 0;
        const Fw::SerializeStatus status = esb.deserializeTo(value);
        ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(value, static_cast<U8>(i));
    }
    ASSERT_EQ(esb.getDeserializeSizeLeft(), 0);
}

void deserializeFail(Fw::ExternalSerializeBuffer& esb) {
    U8 value = 0;
    ASSERT_EQ(esb.getDeserializeSizeLeft(), 0);
    const Fw::SerializeStatus status = esb.deserializeTo(value);
    ASSERT_EQ(status, Fw::FW_DESERIALIZE_BUFFER_EMPTY);
}

TEST(ExternalSerializeBuffer, Basic) {
    Fw::ExternalSerializeBuffer esb(buffer, BUFFER_SIZE);
    // Serialization should succeed
    serializeOK(esb);
    // Serialization should fail
    serializeFail(esb);
    // Deserialization should succeed
    deserializeOK(esb);
    // Deserialization should fail
    deserializeFail(esb);
}

TEST(ExternalSerializeBuffer, Clear) {
    Fw::ExternalSerializeBuffer esb(buffer, BUFFER_SIZE);
    // Serialization should succeed
    serializeOK(esb);
    // Clear the buffer
    esb.clear();
    // Serialization should fail
    serializeFail(esb);
    // Deserialization should fail
    deserializeFail(esb);
}

TEST(ExternalSerializeBuffer, SetExtBuffer) {
    Fw::ExternalSerializeBuffer esb(buffer, BUFFER_SIZE);
    // Serialization should succeed
    serializeOK(esb);
    // Set the buffer
    // This should also clear the serialization state
    esb.setExtBuffer(buffer, BUFFER_SIZE);
    // Serialization should succeed
    serializeOK(esb);
    // Deserialization should succeed
    deserializeOK(esb);
}

TEST(ExternalSerializeBufferWithMemberCopy, Assign) {
    Fw::ExternalSerializeBufferWithMemberCopy esb1(buffer, BUFFER_SIZE);
    Fw::ExternalSerializeBufferWithMemberCopy esb2(buffer, BUFFER_SIZE);
    // Serialization should succeed
    serializeOK(esb1);
    // Assign esb2 to esb1
    esb1 = esb2;
    // Deserialization should fail
    deserializeFail(esb1);
    // Serialization should succeed
    serializeOK(esb1);
}

}  // namespace ExternalSerializeBufferTest

// ----------------------------------------------------------------------
// LinearBufferBase constructor contract
// ----------------------------------------------------------------------

namespace LinearBufferBaseCtorTest {

U8 backing[4];

// A valid pointer with zero capacity is a legal, empty buffer. Ingest paths build one
// when a packet carries a descriptor but no body, so it must not assert.
TEST(LinearBufferBase, ValidPointerZeroCapacity) {
    Fw::ExternalSerializeBuffer esb(backing, 0);
    ASSERT_EQ(esb.getCapacity(), 0);
    ASSERT_EQ(esb.getSize(), 0);
    const U8 value = 1;
    ASSERT_EQ(esb.serializeFrom(value), Fw::FW_SERIALIZE_NO_ROOM_LEFT);
    U8 out = 0;
    ASSERT_EQ(esb.deserializeTo(out), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
}

TEST(SerialBuffer, ValidPointerZeroCapacity) {
    Fw::SerialBuffer sb(backing, 0);
    sb.fill();
    ASSERT_EQ(sb.getCapacity(), 0);
    ASSERT_EQ(sb.getSize(), 0);
    U8 out = 0;
    ASSERT_EQ(sb.deserializeTo(out), Fw::FW_DESERIALIZE_BUFFER_EMPTY);
}

// Null with zero capacity remains legal (used for initialization and reset)
TEST(LinearBufferBase, NullPointerZeroCapacity) {
    Fw::ExternalSerializeBuffer esb(nullptr, 0);
    ASSERT_EQ(esb.getCapacity(), 0);
    ASSERT_EQ(esb.getSize(), 0);
}

// Null with nonzero capacity is the only inconsistent pairing and must still assert
TEST(LinearBufferBaseDeathTest, NullPointerNonzeroCapacity) {
    ASSERT_DEATH(
        {
            Fw::ExternalSerializeBuffer esb(nullptr, 1);
            (void)esb;
        },
        ".*");
}

}  // namespace LinearBufferBaseCtorTest
