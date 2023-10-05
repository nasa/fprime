// ======================================================================
// \title  DeframerTester.hpp
// \brief  Header file for Deframer test with F Prime protocol
// \author mstarch, bocchino
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef SVC_TESTER_HPP
#define SVC_TESTER_HPP

#include <deque>
#include <cstring>

#include "DeframerGTestBase.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "STest/STest/Pick/Pick.hpp"
#include "Svc/Deframer/Deframer.hpp"
#include "Svc/FramingProtocol/FprimeProtocol.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

    class DeframerTester : public DeframerGTestBase {

        // ----------------------------------------------------------------------
        // Friend classes
        // ----------------------------------------------------------------------

        friend struct GenerateFrames;
        friend class SendBuffer;

        // ----------------------------------------------------------------------
        // Types
        // ----------------------------------------------------------------------

      public:

        //! Enumerated constants
        enum Constants {
            //! The maximum valid frame size
            //! Every valid frame must fit in the ring buffer
            MAX_VALID_FRAME_SIZE = DeframerCfg::RING_BUFFER_SIZE,
            //! The max frame size that will fit in the test buffer
            //! Larger than max valid size, to test bad sizes
            MAX_FRAME_SIZE = MAX_VALID_FRAME_SIZE + 1,
            //! The size of the part of the frame that is outside the packet
            NON_PACKET_SIZE = FpFrameHeader::SIZE + HASH_DIGEST_LENGTH,
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

        //! An uplink frame for testing
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
            );

          public:

            // ----------------------------------------------------------------------
            // Public instance methods
            // ----------------------------------------------------------------------

            //! Copy data from the frame, advancing the copy offset
            void copyDataOut(
                Fw::SerialBuffer& serialBuffer, //!< The serial buffer to copy to
                U32 size //!< The number of bytes to copy
            );

            //! Get a constant reference to the frame data
            const FrameData& getData() const;

            //! Get the frame size
            U32 getSize() const;

            //! Get the size of data that remains for copying
            U32 getRemainingCopySize() const;

            //! Report whether the frame is valid
            bool isValid() const;

          public:

            // ----------------------------------------------------------------------
            // Public static methods
            // ----------------------------------------------------------------------

            //! Construct a random frame
            static UplinkFrame random();

            //! Get the max packet size that will fit in the test buffer
            //! This is an invalid size for the deframer
            static U32 getInvalidPacketSize();

            //! Get the max valid command packet size
            static U32 getMaxValidCommandPacketSize();

            //! Get the max valid file packet size
            static U32 getMaxValidFilePacketSize();

            //! Get the min packet size
            static U32 getMinPacketSize();

          private:

            // ----------------------------------------------------------------------
            // Private instance methods
            // ----------------------------------------------------------------------

            //! Randomly invalidate a valid frame, or leave it alone
            //! If the frame is already invalid, leave it alone
            void randomlyInvalidate();

            //! Update the frame header
            void updateHeader();

            //! Update the hash value
            void updateHash();

            //! Write an arbitrary packet size
            void writePacketSize(
                FpFrameHeader::TokenType ps //!< The packet size
            );

            //! Write an arbitrary packet type
            void writePacketType(
                FwPacketDescriptorType pt //!< The packet type
            );

            //! Write an arbitrary start word
            void writeStartWord(
                FpFrameHeader::TokenType sw //!< The start word
            );

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

            //! The frame data, including header, packet data, and hash.
            //! The array is big enough to hold a frame larger than the
            //! max valid frame size for the deframer.
            U8 data[MAX_FRAME_SIZE];

            //! The amount of frame data already copied out into a buffer
            U32 copyOffset;

            //! Whether the frame is valid
            bool valid;

        };

      public:

        // ----------------------------------------------------------------------
        // Constructor
        // ----------------------------------------------------------------------

        //! Construct a DeframerTester
        DeframerTester(InputMode::t inputMode);

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

        //! Set up the incoming buffer
        void setUpIncomingBuffer();

        //! Send the incoming buffer
        void sendIncomingBuffer();

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
        // Private member variables
        // ----------------------------------------------------------------------

        //! The component under test
        Deframer component;

        //! The deframing protocol
        Svc::FprimeDeframing protocol;

        //! Frames that the DeframerTester should send to the Deframer
        std::deque<UplinkFrame> m_framesToSend;

        //! Frames that the DeframerTester should receive from the Deframer
        std::deque<UplinkFrame> m_framesToReceive;

        //! Byte store for the incoming buffer
        //! In polling mode, the incoming buffer must fit in the poll buffer
        U8 m_incomingBufferBytes[DeframerCfg::POLL_BUFFER_SIZE];

        //! Serialized frame data to send to the Deframer
        Fw::Buffer m_incomingBuffer;

        //! The input mode
        InputMode::t m_inputMode;

    };

}

#endif
