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
#include "Svc/Deframer/Deframer.hpp"
#include "Svc/FramingProtocol/FprimeProtocol.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

    class Tester : public DeframerGTestBase {
      private:
        friend struct RandomizeRule;
        friend struct DownlinkRule;
        friend struct FileDownlinkRule;
        friend struct SendAvailableRule;
        // ----------------------------------------------------------------------
        // Construction and destruction
        // ----------------------------------------------------------------------

      public:

        //! An uplink frame
        class UplinkFrame {

          public:

            // ----------------------------------------------------------------------
            // Types
            // ----------------------------------------------------------------------

            enum {
                //! The max frame size
                MAX_SIZE = 256
            };

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
                valid(true)
            {
                ::memset(data, 0xFF, sizeof data);
                this->updateHeader();
                this->updateHash();
            }

          public:

            // ----------------------------------------------------------------------
            // Publilc Methods 
            // ----------------------------------------------------------------------

            //! Get the frame size
            U32 getSize() const {
                return FpFrameHeader::SIZE + packetSize + HASH_DIGEST_LENGTH;
            }

          private:

            // ----------------------------------------------------------------------
            // Private Methods
            // ----------------------------------------------------------------------
            
            //! Update the frame header
            void updateHeader() {
                Fw::SerialBuffer sb(data, sizeof data);
                // Write start word
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
            // Member variables
            // ----------------------------------------------------------------------

            //! The packet type
            const Fw::ComPacket::ComPacketType packetType;

            //! The packet size
            const U32 packetSize;

            //! The amount of frame data already copied out into a buffer
            U32 copyOffset;

            //! The frame data, including header, packet data, and CRC
            U8 data[MAX_SIZE];

            //! Whether the frame is valid
            bool valid;

        };

        //! Construct object Tester
        Tester(bool polling=false);

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

        //! Deque for sending frames
        std::deque<UplinkFrame> m_sending;

        //! Deque for receiving frames
        std::deque<UplinkFrame> m_receiving;

        //! Buffer to hold frames
        Fw::Buffer m_incoming_buffer;

        //! Whether we are in polling mode
        bool m_polling;

    };

}

#endif
