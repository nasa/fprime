#include <FpConfig.hpp>
#include <gtest/gtest.h>

#include "Fw/Types/Serializable.hpp"

namespace ExternalSerializeBufferTest {

  using SizeType = Fw::Serializable::SizeType;

  constexpr SizeType BUFFER_SIZE = 10;

  U8 buffer[BUFFER_SIZE];

  void serializeOK(Fw::ExternalSerializeBuffer& esb) {
    const SizeType buffCapacity = esb.getBuffCapacity();
    ASSERT_EQ(esb.getBuffLength(), 0);
    for (SizeType i = 0; i < buffCapacity; i++) {
      const U8 value = static_cast<U8>(i);
      const Fw::SerializeStatus status = esb.serialize(value);
      ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    }
    ASSERT_EQ(esb.getBuffLength(), buffCapacity);
  }

  void serializeFail(Fw::ExternalSerializeBuffer& esb) {
    ASSERT_EQ(esb.getBuffLength(), esb.getBuffCapacity());
    const Fw::SerializeStatus status = esb.serialize(static_cast<U8>(0));
    ASSERT_EQ(status, Fw::FW_SERIALIZE_NO_ROOM_LEFT);
  }

  void deserializeOK(Fw::ExternalSerializeBuffer& esb) {
    const SizeType buffCapacity = esb.getBuffCapacity();
    ASSERT_EQ(esb.getBuffLeft(), buffCapacity);
    for (SizeType i = 0; i < buffCapacity; i++) {
      U8 value = 0;
      const Fw::SerializeStatus status = esb.deserialize(value);
      ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
      ASSERT_EQ(value, static_cast<U8>(i));
    }
    ASSERT_EQ(esb.getBuffLeft(), 0);
  }

  void deserializeFail(Fw::ExternalSerializeBuffer& esb) {
    U8 value = 0;
    ASSERT_EQ(esb.getBuffLeft(), 0);
    const Fw::SerializeStatus status = esb.deserialize(value);
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

}
