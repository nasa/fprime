// ======================================================================
// \title  DeframingTester.cpp
// \author bocchino
// \brief  cpp file for DeframingTester class
// ======================================================================

#include <cstring>

#include "Fw/Types/SerialBuffer.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/FramingProtocol/test/ut/DeframingTester.hpp"
#include "Utils/Hash/Hash.hpp"
#include "gtest/gtest.h"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction
  // ----------------------------------------------------------------------

  DeframingTester ::
    DeframingTester(U32 cbStoreSize) :
      frameSize(0),
      cbStorage(new U8[cbStoreSize]),
      circularBuffer(this->cbStorage, cbStoreSize),
      interface(*this)
  {
    this->fprimeDeframing.setup(this->interface);
    memset(this->bufferStorage, 0, sizeof this->bufferStorage);
    memset(this->frameData, 0, sizeof this->frameData);
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

  Fw::ByteArray DeframingTester ::
    constructRandomFrame(U32 packetSize)
  {
    FW_ASSERT(packetSize <= MAX_PACKET_SIZE, packetSize, MAX_PACKET_SIZE);
    Fw::SerialBuffer sb(this->frameData, sizeof this->frameData);
    Fw::SerializeStatus status = Fw::FW_SERIALIZE_OK;
    // Serialize the start word
    status = sb.serialize(Svc::FpFrameHeader::START_WORD);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    // Serialize the packet size
    status = sb.serialize(static_cast<FpFrameHeader::TokenType>(packetSize));
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    // Construct the packet data
    for (U32 i = 0; i < packetSize; ++i) {
      const U8 byte = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
      status = sb.serialize(byte);
      FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
    }
    const U32 buffLength = sb.getBuffLength();
    const U32 dataSize = FpFrameHeader::SIZE + packetSize;
    FW_ASSERT(buffLength == dataSize, buffLength, dataSize);
    // Compute the hash value
    Utils::Hash hash;
    Utils::HashBuffer hashBuffer;
    hash.init();
    hash.update(&this->frameData, dataSize);
    hash.final(hashBuffer);
    // Copy the hash value into place
    const U8 *const buffAddr = hashBuffer.getBuffAddr();
    this->frameSize = dataSize + HASH_DIGEST_LENGTH;
    FW_ASSERT(this->frameSize <= MAX_FRAME_SIZE);
    memcpy(&this->frameData[dataSize], buffAddr, HASH_DIGEST_LENGTH);
    // Return the byte array
    return Fw::ByteArray(this->frameData, this->frameSize);
  }

  void DeframingTester ::
    pushFrameOntoCB(Fw::ByteArray frame)
  {
    const Fw::SerializeStatus status =
      this->circularBuffer.serialize(frame.bytes, frame.size);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK);
  }

  Fw::ByteArray DeframingTester ::
    getFrame()
  {
    return Fw::ByteArray(this->frameData, this->frameSize);
  }

  void DeframingTester ::
      checkPacketData()
  {
    FW_ASSERT(
        this->frameSize <= MAX_FRAME_SIZE,
        this->frameSize,
        MAX_FRAME_SIZE
    );
    FW_ASSERT(
        this->frameSize >= NON_PACKET_DATA_SIZE,
        this->frameSize,
        NON_PACKET_DATA_SIZE
    );
    const U32 packetSize = this->frameSize - NON_PACKET_DATA_SIZE;
    Fw::Buffer buffer = this->interface.getRoutedBuffer();
    ASSERT_EQ(buffer.getSize(), packetSize);
    const int result = memcmp(
        &this->frameData[FpFrameHeader::SIZE],
        buffer.getData(),
        packetSize
    );
    ASSERT_EQ(result, 0);
  }

  void DeframingTester ::
    testNominalDeframing(U32 packetSize)
  {
    const Fw::ByteArray frame = this->constructRandomFrame(packetSize);
    this->pushFrameOntoCB(frame);
    U32 needed;
    const Svc::DeframingProtocol::DeframingStatus status =
      this->deframe(needed);
    ASSERT_EQ(status, Svc::DeframingProtocol::DEFRAMING_STATUS_SUCCESS);
    ASSERT_EQ(needed, frame.size);
    this->checkPacketData();
  }

  void DeframingTester ::
    testBadChecksum(U32 packetSize)
  {
    const Fw::ByteArray frame = this->constructRandomFrame(packetSize);
    const U32 hashOffset = FpFrameHeader::SIZE + packetSize;
    ++frame.bytes[hashOffset];
    this->pushFrameOntoCB(frame);
    U32 needed;
    const Svc::DeframingProtocol::DeframingStatus status =
      this->deframe(needed);
    ASSERT_EQ(status, Svc::DeframingProtocol::DEFRAMING_INVALID_CHECKSUM);
  }

}
