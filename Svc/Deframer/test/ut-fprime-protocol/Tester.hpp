// ======================================================================
// \title  GroundInterface/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for GroundInterface test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include <deque>
#include <cstring>

#include "Fw/Com/ComPacket.hpp"
#include "GTestBase.hpp"
#include "Svc/Deframer/Deframer.hpp"
#include "Svc/FramingProtocol/FprimeProtocol.hpp"

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
    struct UplinkFrame {

        // ----------------------------------------------------------------------
        // Types
        // ----------------------------------------------------------------------

        enum {
            //! The max frame size
            MAX_SIZE = 256
        };

        // ----------------------------------------------------------------------
        // Constructor
        // ----------------------------------------------------------------------

        UplinkFrame() :
            packetType(Fw::ComPacket::FW_PACKET_UNKNOWN),
            packetSize(0),
            copyOffset(0),
            size(0),
            valid(false)
        {
            memset(data, 0, sizeof data);
        }

        // ----------------------------------------------------------------------
        // Member variables
        // ----------------------------------------------------------------------

        //! The packet type
        Fw::ComPacket::ComPacketType packetType;
        //! The packet size
        U32 packetSize;
        //! The amount of frame data already copied out into a buffer
        U32 copyOffset;
        //! The frame size
        U32 size;
        //! The frame data, including header, packet data, and CRC
        U8 data[MAX_SIZE];
        //! Whether the frame is valid
        bool valid;

    };

    //! Construct object Tester
    //!
    Tester(bool polling=false);

    //! Destroy object Tester
    //!
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
    //!
    void from_comOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                             Fw::ComBuffer& data,           /*!< Buffer containing packet data*/
                             U32 context                    /*!< Call context value; meaning chosen by user*/
    );

    //! Handler for from_bufferOut
    //!
    void from_bufferOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                Fw::Buffer& fwBuffer);

    //! Handler for from_bufferAllocate
    //!
    Fw::Buffer from_bufferAllocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                           U32 size);

    //! Handler for from_bufferDeallocate
    //!
    void from_bufferDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                       Fw::Buffer& fwBuffer);

    //! Handler for from_framedDeallocate
    //!
    void from_framedDeallocate_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                       Fw::Buffer& fwBuffer);

    //! Handler for from_framedPoll
    //!
    Drv::PollStatus from_framedPoll_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                            Fw::Buffer& pollBuffer);

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts();

    //! Initialize components
    //!
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
    //!
    Deframer component;
    Svc::FprimeDeframing protocol;

    std::deque<UplinkFrame> m_sending;
    std::deque<UplinkFrame> m_receiving;
    Fw::Buffer m_incoming_buffer;
    bool m_polling;

};

}  // end namespace Svc

#endif
