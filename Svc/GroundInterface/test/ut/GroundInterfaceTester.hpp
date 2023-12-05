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

#include <Fw/Com/ComPacket.hpp>
#include "GroundInterfaceGTestBase.hpp"
#include "Svc/GroundInterface/GroundInterface.hpp"

namespace Svc {

  class GroundInterfaceTester :
    public GroundInterfaceGTestBase
  {
    private:
      friend struct RandomizeRule;
      friend struct DownlinkRule;
      friend struct FileDownlinkRule;
      friend struct SendAvailableRule;
      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:
      //! Construct object GroundInterfaceTester
      //!
      GroundInterfaceTester();

      //! Destroy object GroundInterfaceTester
      //!
      ~GroundInterfaceTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      void update_header_info();

      void setInputParams(TOKEN_TYPE size, U8* buffer, TOKEN_TYPE packet_type = Fw::ComPacket::FW_PACKET_UNKNOWN);

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_fileUplinkBufferSendOut
      //!
      void from_fileUplinkBufferSendOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

      //! Handler for from_uplinkPort
      //!
      void from_uplinkPort_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Handler for from_fileDownlinkBufferSendOut
      //!
      void from_fileDownlinkBufferSendOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

      //! Handler for from_fileUplinkBufferGet
      //!
      Fw::Buffer from_fileUplinkBufferGet_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 size
      );

      //! Handler for from_write
      //!
      void from_write_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

      //! Handler for from_readPoll
      //!
      void from_readPoll_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

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
      GroundInterfaceComponentImpl component;

      //! Expected buffer, for checking of the interface
      TOKEN_TYPE m_size;
      TOKEN_TYPE m_packet;
      Fw::Buffer m_incoming_buffer;
      Fw::Buffer m_incoming_file_buffer;
      U8* m_buffer;
      U32 m_uplink_type;
      U32 m_uplink_used;
      U32 m_uplink_size;
      U32 m_uplink_point;
      Fw::ComPacket::ComPacketType m_uplink_com_type;
      // Initialize to empty list to appease valgrind
      U8 m_uplink_data[(sizeof(TOKEN_TYPE) * 3) + FW_COM_BUFFER_MAX_SIZE] = {};

  };

} // end namespace Svc

#endif
