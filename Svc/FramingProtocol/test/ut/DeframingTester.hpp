// ======================================================================
// \title  DeframingTester.hpp
// \author bocchino
// \brief  hpp file for DeframingTester class
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Svc/FramingProtocol/FprimeProtocol.hpp"
#include "Svc/FramingProtocol/DeframingProtocol.hpp"
#include "Utils/Hash/Hash.hpp"
#include "Utils/Types/CircularBuffer.hpp"

namespace Svc {

  //! A harness for checking deframing
  class DeframingTester {

    private:

      // ----------------------------------------------------------------------
      // Constants and types
      // ----------------------------------------------------------------------

#if 0
      //! The serialized packet type
      typedef I32 SerialPacketType;
#endif

      //! Constants
      enum Constants {
        //! The maximum buffer size
        MAX_BUFFER_SIZE = 1024,
        //! The maximum allowed packet size
        MAX_DATA_SIZE = MAX_BUFFER_SIZE -
          sizeof FpFrameHeader::START_WORD -
          HASH_DIGEST_LENGTH,
#if 0
        //! The offset of the start word in an F Prime protocol frame
        START_WORD_OFFSET = 0,
        //! The offset of the packet size in an F Prime protocol frame
        PACKET_SIZE_OFFSET = START_WORD_OFFSET +
            sizeof FpFrameHeader::START_WORD,
        //! The offset of the packet type in an F Prime protocol frame
        PACKET_TYPE_OFFSET = FpFrameHeader::SIZE,
#endif
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
            deframingTester(deframingTester),
            routedBuffer(nullptr)
          {

          }

        public:

          //! Allocate the buffer
          Fw::Buffer allocate(const U32 size) {
            FW_ASSERT(size <= MAX_BUFFER_SIZE);
            Fw::Buffer buffer(this->deframingTester.bufferStorage, size);
            return buffer;
          }

          //! Route the buffer
          void route(Fw::Buffer& data) {
            this->routedBuffer = &data;
          }

          //! Get the routed buffer
          Fw::Buffer *getRoutedBuffer() {
            return this->routedBuffer;
          }

        private:

          //! The enclosing DeframingTester
          DeframingTester& deframingTester;

          //! The routed buffer
          Fw::Buffer *routedBuffer;

      };

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct a DeframingTester
      DeframingTester(
          U32 cbStoreSize = MAX_BUFFER_SIZE //!< The circular buffer store size
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

#if 0
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
#endif

    private:

      // ----------------------------------------------------------------------
      // Private member variables
      // ----------------------------------------------------------------------

      //! Storage for the buffer
      U8 bufferStorage[MAX_BUFFER_SIZE];
      
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
