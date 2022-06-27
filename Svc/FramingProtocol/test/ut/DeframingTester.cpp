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

  void DeframingTester ::
    serializeRandomPacket(U32 packetSize)
  {
    FW_ASSERT(packetSize <= MAX_PACKET_SIZE, packetSize, MAX_PACKET_SIZE);
    // Start word
    this->serializeTokenType(Svc::FpFrameHeader::START_WORD);
    // Packet size
    this->serializeTokenType(packetSize);
    // Packet data
    // TODO
    // Hash value
    // TODO
  }

}
