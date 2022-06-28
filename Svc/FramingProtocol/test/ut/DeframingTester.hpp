// ======================================================================
// \title  DeframingTester.hpp
// \author bocchino
// \brief  hpp file for DeframingTester class
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/ByteArray.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Svc/FramingProtocol/FprimeProtocol.hpp"
#include "Svc/FramingProtocol/DeframingProtocol.hpp"
#include "Utils/Hash/Hash.hpp"
#include "Utils/Types/CircularBuffer.hpp"

namespace Svc {

  //! A harness for checking deframing
  class DeframingTester {

    public:

      // ----------------------------------------------------------------------
      // Constants and types
      // ----------------------------------------------------------------------

      //! Constants
      enum Constants {
        //! The maximum frame size
        MAX_FRAME_SIZE = 1024,
        //! The size of non-packet data in a frame
        NON_PACKET_DATA_SIZE = FpFrameHeader::SIZE + HASH_DIGEST_LENGTH,
        //! The maximum allowed packet size
        MAX_PACKET_SIZE = MAX_FRAME_SIZE - NON_PACKET_DATA_SIZE,
        //! The offset of the start word in an F Prime protocol frame
        START_WORD_OFFSET = 0,
        //! The offset of the packet size in an F Prime protocol frame
        PACKET_SIZE_OFFSET = START_WORD_OFFSET +
          sizeof FpFrameHeader::START_WORD,
      };

      //! The deframing protocol interface
      class Interface :
        public DeframingProtocolInterface
      {

        public:

          //! Construct an Interface
          Interface(
              DeframingTester& deframingTester //!< The enclosing DeframingTester
          ) :
            deframingTester(deframingTester)
          {

          }

        public:

          //! Allocate the buffer
          Fw::Buffer allocate(const U32 size) {
            FW_ASSERT(size <= MAX_FRAME_SIZE);
            Fw::Buffer buffer(this->deframingTester.bufferStorage, size);
            return buffer;
          }

          //! Route the buffer
          void route(Fw::Buffer& data) {
            this->routedBuffer = data;
          }

          //! Get the routed buffer
          Fw::Buffer getRoutedBuffer() {
            return this->routedBuffer;
          }

        private:

          //! The enclosing DeframingTester
          DeframingTester& deframingTester;

          //! The routed buffer
          Fw::Buffer routedBuffer;

      };

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct a DeframingTester
      DeframingTester(
          U32 cbStoreSize = MAX_FRAME_SIZE //!< The circular buffer store size
      );

      //! Destroy a DeframingTester
      ~DeframingTester();

    public:

      // ----------------------------------------------------------------------
      // Public member functions
      // ----------------------------------------------------------------------

      //! Call the deframe function of the deframer
      DeframingProtocol::DeframingStatus deframe(
          U32& needed //!< The number of bytes needed (output)
      );

      //! Serialize a value of token type into the circular buffer
      void serializeTokenType(
          FpFrameHeader::TokenType v //!< The value
      );

      //! Construct a random frame
      //! \return An array pointing to frame data owned by DeframingTester.
      Fw::ByteArray constructRandomFrame(
          U32 packetSize //!< The packet size
      );

      //! Push a frame onto the circular buffer
      void pushFrameOntoCB(
          Fw::ByteArray frame //!< The frame
      );

      //! Get the stored frame
      //! \return The frame
      Fw::ByteArray getFrame();

      //! Check the packet data
      void checkPacketData();

      //! Test nominal deframing with a valid frame containing random
      //! packet data
      void testNominalDeframing(
          U32 packetSize //!< The packet size
      );

      //! Test deframing with a frame containing a bad checksum
      void testBadChecksum(
          U32 packetSize //!< The packet size
      );

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! Storage for the buffer
      U8 bufferStorage[MAX_FRAME_SIZE];

      //! The frame data
      U8 frameData[MAX_FRAME_SIZE];

      //! The frame size
      U32 frameSize;

      //! Storage for the circular buffer
      U8* cbStorage;

      //! The circular buffer
      Types::CircularBuffer circularBuffer;

      //! The framing protocol interface
      Interface interface;

      //! The F Prime framing protocol
      FprimeDeframing fprimeDeframing;

  };

}
