#include <FpConfig.hpp>
#include <gtest/gtest.h>

#include "Fw/Types/Serializable.hpp"

namespace ExternalSerializeBufferTest {

  using SizeType = Fw::Serializable::SizeType;

  constexpr SizeType BUFFER_SIZE = 10;

  U8 buffer[BUFFER_SIZE];

  void serializeOK(Fw::ExternalSerializeBuffer& esb) {
    for (SizeType i = 0; i < BUFFER_SIZE; i++) {
      const U8 value = static_cast<U8>(i);
      const Fw::SerializeStatus status = esb.serialize(value);
      ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
    }
  }

  void serializeFail(Fw::ExternalSerializeBuffer& esb) {
    const Fw::SerializeStatus status = esb.serialize(static_cast<U8>(0));
    ASSERT_EQ(status, Fw::FW_SERIALIZE_NO_ROOM_LEFT);
  }

  void deserializeOK(Fw::ExternalSerializeBuffer& esb) {
    for (SizeType i = 0; i < BUFFER_SIZE; i++) {
      U8 value = 0;
      const Fw::SerializeStatus status = esb.deserialize(value);
      ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
      ASSERT_EQ(value, static_cast<U8>(i));
    }
  }

  void deserializeFail(Fw::ExternalSerializeBuffer& esb) {
    U8 value = 0;
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
    // Derialization should fail
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
    // Derialization should fail
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
    // Derialization should succeed
    deserializeOK(esb);
  }


}
