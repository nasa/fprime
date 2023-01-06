// ======================================================================
// \title  FramingTester.cpp
// \author bocchino
// \brief  cpp file for FramingTester class
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "Svc/FramingProtocol/test/ut/FramingTester.hpp"
#include "gtest/gtest.h"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction
  // ----------------------------------------------------------------------

  FramingTester ::
    FramingTester(Fw::ComPacket::ComPacketType packetType) :
      // Pick a random data size
      dataSize(STest::Pick::lowerUpper(1, MAX_DATA_SIZE)),
      packetType(packetType),
      interface(*this)
  {
    FW_ASSERT(this->dataSize <= MAX_DATA_SIZE);
    this->fprimeFraming.setup(this->interface);
    // Fill in random data
    for (U32 i = 0; i < sizeof(this->data); ++i) {
      this->data[i] = STest::Pick::lowerUpper(0, 0xFF);
    }
    memset(this->bufferStorage, 0, sizeof this->bufferStorage);
  }

  // ----------------------------------------------------------------------
  // Public member functions
  // ----------------------------------------------------------------------

  void FramingTester ::
    check()
  {
    this->fprimeFraming.frame(
        this->data,
        this->dataSize,
        this->packetType
    );
    // Check that we received a buffer
    Fw::Buffer *const sentBuffer = this->interface.getSentBuffer();
    ASSERT_NE(sentBuffer, nullptr);
    if (sentBuffer != nullptr) {
      // Check the start word
      this->checkStartWord();
      // Check the packet size
      const U32 packetSize = this->getPacketSize();
      this->checkPacketSize(packetSize);
      // Check the packet type
      if (this->packetType != Fw::ComPacket::FW_PACKET_UNKNOWN) {
        this->checkPacketType();
      }
      // Check the data
      this->checkData();
      // Check the hash value
      this->checkHash(packetSize);
    }
  }

  // ----------------------------------------------------------------------
  // Private member functions
  // ----------------------------------------------------------------------

  FpFrameHeader::TokenType FramingTester ::
    getPacketSize()
  {
    FpFrameHeader::TokenType packetSize = 0;
    Fw::SerialBuffer sb(
        &this->bufferStorage[PACKET_SIZE_OFFSET],
        sizeof packetSize
    );
    sb.fill();
    const Fw::SerializeStatus status = sb.deserialize(packetSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    return packetSize;
  }

  void FramingTester ::
    checkPacketSize(FpFrameHeader::TokenType packetSize)
  {
    U32 expectedPacketSize = this->dataSize;
    if (this->packetType != Fw::ComPacket::FW_PACKET_UNKNOWN) {
      // Packet type is stored in header
      expectedPacketSize += sizeof(SerialPacketType);
    }
    ASSERT_EQ(packetSize, expectedPacketSize);
  }

  void FramingTester ::
    checkPacketType()
  {
    SerialPacketType serialPacketType = 0;
    Fw::SerialBuffer sb(
        &this->bufferStorage[PACKET_TYPE_OFFSET],
        sizeof serialPacketType
    );
    sb.fill();
    const Fw::SerializeStatus status = sb.deserialize(serialPacketType);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    typedef Fw::ComPacket::ComPacketType PacketType;
    const PacketType pt = static_cast<PacketType>(serialPacketType);
    ASSERT_EQ(pt, this->packetType);
  }

  void FramingTester ::
    checkStartWord()
  {
    FpFrameHeader::TokenType startWord = 0;
    Fw::SerialBuffer sb(
        &this->bufferStorage[START_WORD_OFFSET],
        sizeof startWord
    );
    sb.fill();
    const Fw::SerializeStatus status = sb.deserialize(startWord);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    ASSERT_EQ(startWord, FpFrameHeader::START_WORD);
  }

  void FramingTester ::
    checkData()
  {
    U32 dataOffset = PACKET_TYPE_OFFSET;
    if (this->packetType != Fw::ComPacket::FW_PACKET_UNKNOWN) {
      // Packet type is stored in header
      dataOffset += sizeof(SerialPacketType);
    }
    const I32 result = memcmp(
        this->data,
        &this->bufferStorage[dataOffset],
        this->dataSize
    );
    ASSERT_EQ(result, 0);
  }

  void FramingTester ::
    checkHash(FpFrameHeader::TokenType packetSize)
  {
    Utils::Hash hash;
    Utils::HashBuffer hashBuffer;
    const U32 dataSize = FpFrameHeader::SIZE + packetSize;
    hash.update(this->bufferStorage,  dataSize);
    hash.final(hashBuffer);
    const U8 *const hashAddr = hashBuffer.getBuffAddr();
    const I32 result = memcmp(
        &this->bufferStorage[dataSize],
        hashAddr,
        HASH_DIGEST_LENGTH
    );
    ASSERT_EQ(result, 0);
  }

}
