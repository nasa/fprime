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
#include <ComPacket.hpp>
#include "GTestBase.hpp"
#include "Svc/Deframer/DeframerComponentImpl.hpp"
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
    struct UplinkData {
        Fw::ComPacket::ComPacketType type;
        U32 size;
        U32 partial;
        U32 full_size;
        bool corrupted;
        U8 data[FW_COM_BUFFER_MAX_SIZE];
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

    void update_header_info(U32 garbage_index, U8 garbage_byte);

    void setInputParams(FP_FRAME_TOKEN_TYPE size,
                        U8* buffer,
                        FP_FRAME_TOKEN_TYPE packet_type = Fw::ComPacket::FW_PACKET_UNKNOWN);

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

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    DeframerComponentImpl component;
    Svc::FprimeDeframing protocol;

    std::deque<UplinkData> m_sending;
    std::deque<UplinkData> m_receiving;
    Fw::Buffer m_incoming_buffer;
    bool m_polling;

    //! Expected buffer, for checking of the interface
    //FP_FRAME_TOKEN_TYPE m_size;
    //FP_FRAME_TOKEN_TYPE m_packet;
    //
    //Fw::Buffer m_incoming_file_buffer;


    /*U8* m_buffer;
    U32 m_uplink_type;
    U32 m_uplink_used;
    U32 m_uplink_size;
    U32 m_uplink_point;
    bool m_garbage;
    Fw::ComPacket::ComPacketType m_uplink_com_type;
    // Initialize to empty list to appease valgrind
    U8 m_uplink_data[(sizeof(FP_FRAME_TOKEN_TYPE) * 2) + sizeof(U32) + FW_COM_BUFFER_MAX_SIZE] = {};*/
};

}  // end namespace Svc

#endif
