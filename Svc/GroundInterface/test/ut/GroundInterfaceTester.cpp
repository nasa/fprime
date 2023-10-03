// ======================================================================
// \title  GroundInterface.hpp
// \author mstarch
// \brief  cpp file for GroundInterface test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "GroundInterfaceTester.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 10

U8 file_back_buffer[10240];

namespace Svc {
  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  GroundInterfaceTester ::
    GroundInterfaceTester() :
      GroundInterfaceGTestBase("Tester", MAX_HISTORY_SIZE),
      component("GroundInterface")
      ,
      m_buffer(nullptr),
      m_uplink_type(1),
      m_uplink_used(30),
      m_uplink_size(sizeof(TOKEN_TYPE) * 3 + m_uplink_used),
      m_uplink_point(0),
      m_uplink_com_type(Fw::ComPacket::FW_PACKET_COMMAND)
  {
    this->initComponents();
    this->connectPorts();
    update_header_info();
  }

  GroundInterfaceTester ::
    ~GroundInterfaceTester()
  {

  }



  // ----------------------------------------------------------------------
  // Tests State Adjustments
  // ----------------------------------------------------------------------

  void GroundInterfaceTester :: update_header_info() {
      // Write token types
      for (U32 i = 0; i < sizeof(TOKEN_TYPE); i++) {
          m_uplink_data[i] = (GroundInterfaceComponentImpl::START_WORD >> ((sizeof(TOKEN_TYPE) - 1 - i) * 8)) & 0xFF;
          m_uplink_data[i + sizeof(TOKEN_TYPE)] =
                  (m_uplink_used >> ((sizeof(TOKEN_TYPE) - 1 - i) * 8)) & 0xFF;
          m_uplink_data[i + 2 * sizeof(TOKEN_TYPE) + m_uplink_used] =
                  (GroundInterfaceComponentImpl::END_WORD >> ((sizeof(TOKEN_TYPE) - 1 - i) * 8)) & 0xFF;
      }
      // Packet type
      m_uplink_data[2 * sizeof(TOKEN_TYPE) + 0] = (static_cast<U32>(m_uplink_com_type) >> 24) & 0xFF;
      m_uplink_data[2 * sizeof(TOKEN_TYPE) + 1] = (static_cast<U32>(m_uplink_com_type) >> 16) & 0xFF;
      m_uplink_data[2 * sizeof(TOKEN_TYPE) + 2] = (static_cast<U32>(m_uplink_com_type) >>  8) & 0xFF;
      m_uplink_data[2 * sizeof(TOKEN_TYPE) + 3] = (static_cast<U32>(m_uplink_com_type) >>  0) & 0xFF;
  }

  void GroundInterfaceTester :: setInputParams(TOKEN_TYPE size, U8* buffer, TOKEN_TYPE packet_type) {
      m_size = size;
      m_buffer = buffer;
      m_packet = packet_type;
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void GroundInterfaceTester ::
    from_fileUplinkBufferSendOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_fileUplinkBufferSendOut(fwBuffer);
    for (U32 i = 0; i < fwBuffer.getSize(); i++) {
        // File uplink strips type before outputting to FileUplink
        ASSERT_EQ(fwBuffer.getData()[i], m_uplink_data[i + HEADER_SIZE + sizeof(TOKEN_TYPE)]);
    }
  }

  void GroundInterfaceTester ::
    from_uplinkPort_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    this->pushFromPortEntry_uplinkPort(data, context);
    for (U32 i = 0; i < data.getBuffLength(); i++) {
        ASSERT_EQ(data.getBuffAddr()[i], m_uplink_data[i + HEADER_SIZE]);
    }
  }

  void GroundInterfaceTester ::
    from_fileDownlinkBufferSendOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_fileDownlinkBufferSendOut(fwBuffer);

  }

  Fw::Buffer GroundInterfaceTester ::
    from_fileUplinkBufferGet_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    this->pushFromPortEntry_fileUplinkBufferGet(size);
    m_incoming_file_buffer.setData(file_back_buffer);
    m_incoming_file_buffer.setSize(size);
    return m_incoming_file_buffer;
  }

  void GroundInterfaceTester ::
    from_write_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_write(fwBuffer);
    // All writes can be processed here
    TOKEN_TYPE start = 0;
    TOKEN_TYPE size = 0;
    TOKEN_TYPE packet = 0;
    U32 end = 0;
    Fw::ExternalSerializeBuffer buffer_wrapper(fwBuffer.getData(), fwBuffer.getSize());
    buffer_wrapper.setBuffLen(fwBuffer.getSize());
    // Check basic deserialization
    ASSERT_EQ(buffer_wrapper.deserialize(start), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(buffer_wrapper.deserialize(size), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(start, GroundInterfaceComponentImpl::START_WORD);
    ASSERT_EQ(size, m_size);
    // Deserialize the packet type, if know. This handles the different paths for FilePackets and homogenized packets
    if (m_packet != Fw::ComPacket::FW_PACKET_UNKNOWN) {
        ASSERT_EQ(buffer_wrapper.deserialize(packet), Fw::FW_SERIALIZE_OK);
        // For now, only FilePackets take this path
        ASSERT_EQ(packet, Fw::ComPacket::FW_PACKET_FILE);
        m_size -= sizeof(packet);
    }
    // Note: no checking for symmetric errors, as we are depending on ExternalSerializeBuffer's correctness here
    for (U32 i = 0; i < m_size; i++) {
        U8 byte_data;
        ASSERT_EQ(buffer_wrapper.deserialize(byte_data), Fw::FW_SERIALIZE_OK);
        ASSERT_EQ(byte_data, m_buffer[i]);
    }
    // Look at end work
    ASSERT_EQ(buffer_wrapper.deserialize(end), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(end, GroundInterfaceComponentImpl::END_WORD);
    ASSERT_from_write_SIZE(1);
  }

  void GroundInterfaceTester ::
    from_readPoll_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    this->pushFromPortEntry_readPoll(fwBuffer);
    U8* incoming = m_incoming_buffer.getData();
    U8* outgoing = fwBuffer.getData();
    for (U32 i = 0; i < m_incoming_buffer.getSize(); i++) {
        outgoing[i] = incoming[i];
    }
    fwBuffer.setSize(m_incoming_buffer.getSize());
  }

  // ----------------------------------------------------------------------
  // Helper methods
  // ----------------------------------------------------------------------

  void GroundInterfaceTester ::
    connectPorts()
  {

    // downlinkPort
    this->connect_to_downlinkPort(
        0,
        this->component.get_downlinkPort_InputPort(0)
    );

    // fileDownlinkBufferSendIn
    this->connect_to_fileDownlinkBufferSendIn(
        0,
        this->component.get_fileDownlinkBufferSendIn_InputPort(0)
    );

    // readCallback
    this->connect_to_readCallback(
        0,
        this->component.get_readCallback_InputPort(0)
    );

    // schedIn
    this->connect_to_schedIn(
        0,
        this->component.get_schedIn_InputPort(0)
    );

    // fileUplinkBufferSendOut
    this->component.set_fileUplinkBufferSendOut_OutputPort(
        0,
        this->get_from_fileUplinkBufferSendOut(0)
    );

    // Log
    this->component.set_Log_OutputPort(
        0,
        this->get_from_Log(0)
    );

    // LogText
    this->component.set_LogText_OutputPort(
        0,
        this->get_from_LogText(0)
    );

    // Time
    this->component.set_Time_OutputPort(
        0,
        this->get_from_Time(0)
    );

    // uplinkPort
    this->component.set_uplinkPort_OutputPort(
        0,
        this->get_from_uplinkPort(0)
    );

    // fileDownlinkBufferSendOut
    this->component.set_fileDownlinkBufferSendOut_OutputPort(
        0,
        this->get_from_fileDownlinkBufferSendOut(0)
    );

    // fileUplinkBufferGet
    this->component.set_fileUplinkBufferGet_OutputPort(
        0,
        this->get_from_fileUplinkBufferGet(0)
    );

    // write
    this->component.set_write_OutputPort(
        0,
        this->get_from_write(0)
    );

    // readPoll
    this->component.set_readPoll_OutputPort(
        0,
        this->get_from_readPoll(0)
    );

  }

  void GroundInterfaceTester ::
    initComponents()
  {
    this->init();
    this->component.init(
        INSTANCE
    );
  }

} // end namespace Svc
