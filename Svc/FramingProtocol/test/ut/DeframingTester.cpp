// ======================================================================
// \title  DeframingTester.cpp
// \author bocchino
// \brief  cpp file for DeframingTester class
// ======================================================================

#include "Fw/Types/SerialBuffer.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/FramingProtocol/test/ut/DeframingTester.hpp"
#include "gtest/gtest.h"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction
  // ----------------------------------------------------------------------

  DeframingTester ::
    DeframingTester(U32 cbStoreSize) :
      cbStorage(new U8[cbStoreSize]),
      circularBuffer(this->cbStorage, cbStoreSize), 
      interface(*this)
  {
    this->fprimeDeframing.setup(this->interface);
    memset(this->bufferStorage, 0, sizeof this->bufferStorage);
    memset(this->cbStorage, 0, cbStoreSize);
  }

  DeframingTester ::
    ~DeframingTester()
  {
    delete[](this->cbStorage);
  }

  // ----------------------------------------------------------------------
  // Public member functions
  // ----------------------------------------------------------------------

  DeframingProtocol::DeframingStatus DeframingTester ::
    deframe(U32& needed)
  {
    return this->fprimeDeframing.deframe(this->circularBuffer, needed);
  }

  void DeframingTester ::
    serializeTokenType(FpFrameHeader::TokenType v)
  {
    U8 buffer[sizeof v];
    Fw::SerialBuffer sb(buffer, sizeof buffer);
    {
      const Fw::SerializeStatus status = sb.serialize(v);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    }
    {
      const Fw::SerializeStatus status =
        this->circularBuffer.serialize(buffer, sizeof buffer);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    }
  }

#if 0
  // ----------------------------------------------------------------------
  // Private member functions
  // ----------------------------------------------------------------------

  FpFrameHeader::TokenType DeframingTester ::
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

  void DeframingTester ::
    checkPacketSize(FpFrameHeader::TokenType packetSize)
  {
    U32 expectedPacketSize = this->dataSize;
    if (this->packetType != Fw::ComPacket::FW_PACKET_UNKNOWN) {
      // Packet type is stored in header
      expectedPacketSize += sizeof(SerialPacketType);
    }
    ASSERT_EQ(packetSize, expectedPacketSize);
  }

  void DeframingTester ::
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

  void DeframingTester ::
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

  void DeframingTester ::
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

  void DeframingTester ::
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
#endif

}
