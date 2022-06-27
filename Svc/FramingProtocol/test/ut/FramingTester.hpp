// ======================================================================
// \title  FramingTester.hpp
// \author bocchino
// \brief  hpp file for FramingTester class
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Svc/FramingProtocol/FprimeProtocol.hpp"
#include "Svc/FramingProtocol/FramingProtocol.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

  //! A harness for checking framing
  class FramingTester {

    private:

      // ----------------------------------------------------------------------
      // Constants and types
      // ----------------------------------------------------------------------

      //! The serialized packet type
      typedef I32 SerialPacketType;

      //! Constants
      enum Constants {
        //! The maximum buffer size
        MAX_BUFFER_SIZE = 1024,
        //! The maximum allowed data size
        MAX_DATA_SIZE = MAX_BUFFER_SIZE -
          FpFrameHeader::SIZE -
          sizeof(SerialPacketType) -
          HASH_DIGEST_LENGTH,
        //! The offset of the start word in an F Prime protocol frame
        START_WORD_OFFSET = 0,
        //! The offset of the packet size in an F Prime protocol frame
        PACKET_SIZE_OFFSET = START_WORD_OFFSET +
            sizeof FpFrameHeader::START_WORD,
        //! The offset of the packet type in an F Prime protocol frame
        PACKET_TYPE_OFFSET = FpFrameHeader::SIZE,
      };

      //! The framing protocol interface
      class Interface :
        public FramingProtocolInterface
      {

        public:

          //! Construct an Interface
          Interface(
              FramingTester& framingTester //!< The enclosing FramingTester
          ) :
            framingTester(framingTester),
            sentBuffer(nullptr)
          {

          }

        public:

          //! Allocate the buffer
          Fw::Buffer allocate(const U32 size) {
            FW_ASSERT(size <= MAX_BUFFER_SIZE, size, MAX_BUFFER_SIZE);
            Fw::Buffer buffer(this->framingTester.bufferStorage, size);
            return buffer;
          }

          //! Send the buffer
          void send(Fw::Buffer& outgoing) {
            this->sentBuffer = &outgoing;
          }

          //! Get the sent buffer
          Fw::Buffer *getSentBuffer() {
            return this->sentBuffer;
          }

        private:

          //! The enclosing FramingTester
          FramingTester& framingTester;

          //! The sent buffer
          Fw::Buffer *sentBuffer;

      };

    public:

      // ----------------------------------------------------------------------
      // Construction
      // ----------------------------------------------------------------------

      //! Construct a FramingTester
      FramingTester(
          Fw::ComPacket::ComPacketType packetType //!< The packet type
      );

    public:

      // ----------------------------------------------------------------------
      // Public member functions
      // ----------------------------------------------------------------------

      //! Check framing
      void check();

      // ----------------------------------------------------------------------
      // Private member functions
      // ----------------------------------------------------------------------

    private:

      //! Get the packet size from the buffer
      FpFrameHeader::TokenType getPacketSize();

      //! Check the packet size in the buffer
      void checkPacketSize(
          FpFrameHeader::TokenType packetSize //!< The packet size
      );

      //! Check the packet type in the buffer
      void checkPacketType();

      //! Check the start word in the buffer
      void checkStartWord();

      //! Check the data in the buffer
      void checkData();

      //! Check the hash value in the buffer
      void checkHash(
          FpFrameHeader::TokenType packetSize //!< The packet size
      );

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! The data to frame
      U8 data[MAX_DATA_SIZE];

      //! The data size in bytes
      const U32 dataSize;

      //! The packet type
      Fw::ComPacket::ComPacketType packetType;

      //! Storage for the buffer
      U8 bufferStorage[MAX_BUFFER_SIZE];

      //! The framing protocol interface
      Interface interface;

      //! The F Prime framing protocol
      FprimeFraming fprimeFraming;

  };

}
