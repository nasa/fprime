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
            //! The max frame size
            //! POLL_BUFFER_SIZE is the max size that works with the polling buffer
            MAX_FRAME_SIZE = DeframerCfg::POLL_BUFFER_SIZE
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
                for (U32 i = 0; i < sizeof data; ++i) {
                    data[i] = STest::Pick::lowerUpper(0, 0xFF);
                }
                this->updateHeader();
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

            //! Whether the frame is valid
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
                return (sizeof data) - FpFrameHeader::SIZE - HASH_DIGEST_LENGTH;
            }

            //! Construct a random frame
            static UplinkFrame random() {
                // Randomly set the packet type
                auto packetType = Fw::ComPacket::FW_PACKET_UNKNOWN;
                const U32 packetSelector = STest::Pick::startLength(0,3);
                switch (packetSelector) {
                    case 0:
                        packetType = Fw::ComPacket::FW_PACKET_COMMAND;
                        break;
                    case 1:
                        packetType = Fw::ComPacket::FW_PACKET_FILE;
                        break;
                    default:
                        // Use packet type UNKNOWN
                        break;
                }
                // Randomly set the packet size
                U32 packetSize = STest::Pick::lowerUpper(
                    getMinPacketSize(),
                    getMaxCommandPacketSize()
                );
                // Construct and return the frame
                return UplinkFrame(packetType, packetSize);
            }

          private:

            // ----------------------------------------------------------------------
            // Private methods
            // ----------------------------------------------------------------------
            
            //! Update the frame header
            void updateHeader() {
                Fw::SerialBuffer sb(data, sizeof data);
                // Write the start word
                auto status = sb.serialize(FpFrameHeader::START_WORD);
                ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
                // Write the packet size
                status = sb.serialize(packetSize);
                ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);
                // Write the packet type
                const FwPacketDescriptorType descriptorType = packetType;
                status = sb.serialize(descriptorType);
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
        Tester(InputMode::t polling = InputMode::PUSH);

        //! Destroy object Tester
        ~Tester(void);

      public:

        // ----------------------------------------------------------------------
        // Tests
        // ----------------------------------------------------------------------

        //! Size too large for deframer
        void sizeTooLarge();

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
        // Helper methods
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
