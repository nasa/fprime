// ======================================================================
// \title  Deframer/ut/ut-fprime-protocol/Tester.hpp
// \author mstarch, bocchino
// \brief  hpp file for Deframer test with F Prime protocol
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include <deque>
#include <cstring>

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "GTestBase.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "Svc/Deframer/Deframer.hpp"
#include "Svc/FramingProtocol/FprimeProtocol.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

    class Tester : public DeframerGTestBase {

        // ----------------------------------------------------------------------
        // Friend classes 
        // ----------------------------------------------------------------------

        friend struct GenerateFrames;
        friend struct SendBuffer;

        // ----------------------------------------------------------------------
        // Types
        // ----------------------------------------------------------------------

      public:

        //! Enumerated constants
        enum Constants {
            //! The maximum frame size
            //! Every frame has to fit in the ring buffer
            //! In polling mode, every frame has to fit in the polling buffer
            MAX_FRAME_SIZE = 
              (DeframerCfg::POLL_BUFFER_SIZE < DeframerCfg::RING_BUFFER_SIZE) ?
                  DeframerCfg::POLL_BUFFER_SIZE :
                  DeframerCfg::RING_BUFFER_SIZE,
            //! The offset of the start word in an F Prime protocol frame
            START_WORD_OFFSET = 0,
            //! The offset of the packet size in an F Prime protocol frame
            PACKET_SIZE_OFFSET = START_WORD_OFFSET +
                sizeof FpFrameHeader::START_WORD,
            //! The offset of the packet type in an F Prime protocol frame
            PACKET_TYPE_OFFSET = FpFrameHeader::SIZE,
        };

        //! The type of the input mode
        struct InputMode {
            typedef enum {
                //! Push data from another thread
                PUSH,
                //! Poll for data on the schedIn thread
                POLL
            } t;
        };

        //! An uplink frame
        class UplinkFrame {

            // ----------------------------------------------------------------------
            // Types
            // ----------------------------------------------------------------------

            //! The type of frame data
            typedef U8 FrameData[MAX_FRAME_SIZE];

          public:

            // ----------------------------------------------------------------------
            // Constructor
            // ----------------------------------------------------------------------

            //! Construct an uplink frame
            UplinkFrame(
                Fw::ComPacket::ComPacketType packetType, //!< The packet type
                U32 packetSize //!< The packet size
            ) :
                packetType(packetType),
                packetSize(packetSize),
                copyOffset(0),
                valid(
                    packetType == Fw::ComPacket::FW_PACKET_COMMAND ||
                    packetType == Fw::ComPacket::FW_PACKET_FILE
                )
            {
                // Fill in random data
                for (U32 i = 0; i < sizeof data; ++i) {
                    data[i] = STest::Pick::lowerUpper(0, 0xFF);
                }
                // Update the frame header
                this->updateHeader();
                // Update the hash value
                this->updateHash();
            }

          public:

            // ----------------------------------------------------------------------
            // Public methods 
            // ----------------------------------------------------------------------

            //! Get the frame size
            U32 getSize() const {
                return FpFrameHeader::SIZE + packetSize + HASH_DIGEST_LENGTH;
            }

            //! Get the size of data that remains for copying
            U32 getRemainingCopySize() {
                const U32 frameSize = getSize();
                FW_ASSERT(frameSize >= copyOffset, frameSize, copyOffset);
                return frameSize - copyOffset;
            }

            //! Report whether the frame is valid
            bool isValid() const {
                return valid;
            }

            //! Copy data from the frame, advancing the copy offset
            void copyDataOut(
                Fw::SerialBuffer& serialBuffer, //!< The serial buffer to copy to
                U32 size //!< The number of bytes to copy
            ) {
                ASSERT_LE(copyOffset + size, getSize());
                auto status = serialBuffer.pushBytes(
                    &data[copyOffset],
                    size
                );
                ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
                copyOffset += size;
            }

            //! Get the min packet size
            static U32 getMinPacketSize() {
                // Packet must hold the packet type
                return sizeof(FwPacketDescriptorType);
            }

            //! Get the max packet size
            static U32 getMaxCommandPacketSize() {
                return std::min(
                    // Packet must fit into a com buffer
                    static_cast<U32>(FW_COM_BUFFER_MAX_SIZE),
                    getMaxFilePacketSize()
                );
            }

            //! Get a constant reference to the frame data
            const FrameData& getData() const {
                return data;
            }

            //! Get the max file size
            static U32 getMaxFilePacketSize() {
                // The size of the parts of the frame that are outside the packet
                const U32 nonPacketSize = FpFrameHeader::SIZE + HASH_DIGEST_LENGTH;
                FW_ASSERT(
                    MAX_FRAME_SIZE >= nonPacketSize,
                    MAX_FRAME_SIZE,
                    nonPacketSize
                );
                return MAX_FRAME_SIZE - nonPacketSize;
            }

            //! Construct a random frame
            static UplinkFrame random() {
                // Randomly set the packet type
                auto packetType = Fw::ComPacket::FW_PACKET_UNKNOWN;
                const U32 packetSelector = STest::Pick::lowerUpper(0,1);
                switch (packetSelector) {
                    case 0:
                        packetType = Fw::ComPacket::FW_PACKET_COMMAND;
                        break;
                    case 1:
                        packetType = Fw::ComPacket::FW_PACKET_FILE;
                        break;
                    default:
                        FW_ASSERT(0);
                        break;
                }
                // Randomly set the packet size
                const U32 packetSize = STest::Pick::lowerUpper(
                    getMinPacketSize(),
                    getMaxCommandPacketSize()
                );
                // Construct the frame
                auto frame = UplinkFrame(packetType, packetSize);
                // Randomly invalidate the frame
                frame.randomlyInvalidate();
                // Return the frame
                return frame;
            }

          private:

            // ----------------------------------------------------------------------
            // Private methods
            // ----------------------------------------------------------------------
            
            //! Update the frame header
            void updateHeader() {
                // Write the correct start word
                writeStartWord(FpFrameHeader::START_WORD);
                // Write the correct packet size
                writePacketSize(packetSize);
                // Write the correct packet type
                writePacketType(packetType);
            }

            //! Write an arbitrary start word
            void writeStartWord(FpFrameHeader::TokenType startWord) {
                Fw::SerialBuffer sb(data, sizeof startWord);
                const auto status = sb.serialize(startWord);
                ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
            }

            //! Write an arbitrary packet size
            void writePacketSize(FpFrameHeader::TokenType ps) {
                Fw::SerialBuffer sb(&data[PACKET_SIZE_OFFSET], sizeof ps);
                const auto status = sb.serialize(packetSize);
                ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
            }

            //! Write an arbitrary packet type
            void writePacketType(FwPacketDescriptorType pt) {
                Fw::SerialBuffer sb(&data[PACKET_TYPE_OFFSET], sizeof packetType);
                const auto status = sb.serialize(pt);
                ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
            }

            //! Update the hash value
            void updateHash() {
                Utils::Hash hash;
                Utils::HashBuffer hashBuffer;
                hash.update(data,  FpFrameHeader::SIZE + packetSize);
                hash.final(hashBuffer);
                const U32 hashOffset = getSize() - HASH_DIGEST_LENGTH;
                const U8 *const hashAddr = hashBuffer.getBuffAddr();
                memcpy(&data[hashOffset], hashAddr, HASH_DIGEST_LENGTH);
            }

            //! Randomly invalidate a valid frame
            void randomlyInvalidate() {
                // If frame is already invalid, we should not be here
                ASSERT_TRUE(valid);
                // Select out of 100 samples
                const U32 invalidateIndex = STest::Pick::startLength(0, 100);
                switch (invalidateIndex) {
                    case 0: {
                        // Invalidate the start word
                        const FpFrameHeader::TokenType badStartWord =
                            FpFrameHeader::START_WORD + 1;
                        writeStartWord(badStartWord);
                        valid = false;
                        break;
                    }
                    case 1:
                        // Invalidate the packet type
                        writePacketType(Fw::ComPacket::FW_PACKET_UNKNOWN);
                        valid = false;
                        break;
                    case 2:
                        // Invalidate the hash value
                        ++data[getSize() - 1];
                        valid = false;
                        break;
                    default:
                        // Stay valid
                        break;
                }
            }

          public:

            // ----------------------------------------------------------------------
            // Public member variables
            // ----------------------------------------------------------------------

            //! The packet type
            const Fw::ComPacket::ComPacketType packetType;

            //! The packet size
            const U32 packetSize;

          private:

            // ----------------------------------------------------------------------
            // Private member variables 
            // ----------------------------------------------------------------------

            //! The frame data, including header, packet data, and CRC
            U8 data[MAX_FRAME_SIZE];

            //! The amount of frame data already copied out into a buffer
            U32 copyOffset;

            //! Whether the frame is valid
            bool valid;

        };

      public:

        // ----------------------------------------------------------------------
        // Construction and destruction 
        // ----------------------------------------------------------------------
        
        //! Construct object Tester
        Tester(InputMode::t inputMode);

        //! Destroy object Tester
        ~Tester(void);

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Size would cause integer overflow
        void sizeOverflow();

      public:

        // ----------------------------------------------------------------------
        // Public instance methods 
        // ----------------------------------------------------------------------

        //! Send the incoming buffer
        void sendIncomingBuffer() {
            switch (m_inputMode) {
                case InputMode::PUSH:
                    invoke_to_framedIn(
                        0,
                        m_incomingBuffer,
                        Drv::RecvStatus::RECV_OK
                    );
                    break;
                case InputMode::POLL:
                    // TODO
                    FW_ASSERT(0);
                    break;
                default:
                    FW_ASSERT(0);
                    break;
            }
        }

      private:

        // ----------------------------------------------------------------------
        // Handlers for typed from ports
        // ----------------------------------------------------------------------

        //! Handler for from_comOut
        void from_comOut_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            Fw::ComBuffer& data, //!< Buffer containing packet data
            U32 context //!< Call context value; meaning chosen by user
        );

        //! Handler for from_bufferOut
        void from_bufferOut_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            Fw::Buffer& fwBuffer //!< The buffer
        );

        //! Handler for from_bufferAllocate
        Fw::Buffer from_bufferAllocate_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            U32 size //!< The size
        );

        //! Handler for from_bufferDeallocate
        void from_bufferDeallocate_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            Fw::Buffer& fwBuffer //!< The buffer
        );

        //! Handler for from_framedDeallocate
        //!
        void from_framedDeallocate_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            Fw::Buffer& fwBuffer //!< The buffer
        );

        //! Handler for from_framedPoll
        Drv::PollStatus from_framedPoll_handler(
            const NATIVE_INT_TYPE portNum, //!< The port number
            Fw::Buffer& pollBuffer //!< The poll buffer
        );

      private:

        // ----------------------------------------------------------------------
        // Private helper methods
        // ----------------------------------------------------------------------

        //! Connect ports
        void connectPorts();

        //! Initialize components
        void initComponents();

        //! Allocate a packet buffer
        Fw::Buffer allocatePacketBuffer(
            U32 size //!< The buffer size
        );

      private:

        // ----------------------------------------------------------------------
        // Variables
        // ----------------------------------------------------------------------

        //! The component under test
        Deframer component;

        //! The deframing protocol
        Svc::FprimeDeframing protocol;

        //! Deque for frames to send
        std::deque<UplinkFrame> m_framesToSend;

        //! Deque for receiving frames
        std::deque<UplinkFrame> m_framesReceived;

        //! Bytes for incoming buffer
        U8 m_incomingBufferBytes[MAX_FRAME_SIZE];

        //! Buffer to hold frames
        Fw::Buffer m_incomingBuffer;

        //! The input mode
        InputMode::t m_inputMode;

    };

}

#endif
