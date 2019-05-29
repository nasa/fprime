// ====================================================================== 
// \title  GroundInterface.cpp
// \author lestarch
// \brief  cpp file for GroundInterface component implementation class
// ====================================================================== 


#include <Svc/GroundInterface/GroundInterface.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <string.h>
namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  GroundInterfaceComponentImpl ::
#if FW_OBJECT_NAMES == 1
    GroundInterfaceComponentImpl(
        const char *const compName
    ) :
      GroundInterfaceComponentBase(compName),
#else
  GroundInterfaceComponentBase(void),
#endif
    m_ext_buffer(0xfeedfeed, 0xdeeddeed, reinterpret_cast<POINTER_CAST>(m_buffer), GND_BUFFER_SIZE),
    m_data_size(0),
    m_data_type(MAX_DATA_TYPE),
    m_in_ring(m_in_buffer, GND_BUFFER_SIZE)
  {

  }

  void GroundInterfaceComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    ) 
  {
    GroundInterfaceComponentBase::init(instance);
  }

  GroundInterfaceComponentImpl ::
    ~GroundInterfaceComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void GroundInterfaceComponentImpl ::
    downlinkPort_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
      // TODO: replace with a call to a buffer manager
      Fw::Buffer buffer = m_ext_buffer;
      Fw::ExternalSerializeBuffer buffer_wrapper(reinterpret_cast<U8*>(m_ext_buffer.getdata()),
                                                 m_ext_buffer.getsize());
      // Serialize data
      buffer_wrapper.serialize(START_WORD);
      buffer_wrapper.serialize(static_cast<TOKEN_TYPE>(data.getBuffLength()));
      buffer_wrapper.serialize(data.getBuffAddr(), data.getBuffLength(), true);
      buffer_wrapper.serialize(static_cast<U32>(END_WORD));

      buffer.setsize(buffer_wrapper.getBuffLength());
      buffer.setdata(reinterpret_cast<POINTER_CAST>(buffer_wrapper.getBuffAddr()));
      write_out(0, buffer);
  }

  void GroundInterfaceComponentImpl ::
    fileDownlinkBufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {}

  void GroundInterfaceComponentImpl ::
    readCallback_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &buffer
    )
  {
      processBuffer(buffer);
  }

  void GroundInterfaceComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum, /*!< The port number*/
        NATIVE_UINT_TYPE context /*!< The call order*/
    )
  {
      // TODO: replace with a call to a buffer manager
      Fw::Buffer buffer = m_ext_buffer;
      readPoll_out(0, buffer);
      processBuffer(buffer);

  }

  void GroundInterfaceComponentImpl ::
    routeComData()
  {
      Fw::ComBuffer com;
      m_in_ring.peek(com.getBuffAddr(), m_data_size, HEADER_SIZE);
      com.setBuffLen(m_data_size);
      switch (m_data_type) {
          case COMMAND_TYPE:
              uplinkPort_out(0, com, 0);
              break;
          default:
              //TODO: set error count
              break;
      }
  }

  void GroundInterfaceComponentImpl ::
    processRing()
  {
      // Header items for the packet
      TOKEN_TYPE start;
      TOKEN_TYPE type;
      // Footer items
      U32 checksum;
      // Inner-loop, process ring buffer looking for at least the header
      while (m_in_ring.get_remaining_size() >= HEADER_SIZE) {
          m_data_size = 0;
          // Peek into the header and read out values
          Fw::SerializeStatus status = m_in_ring.peek(start, 0);
          FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
          status = m_in_ring.peek(type, sizeof(TOKEN_TYPE));
          FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
          m_data_type = static_cast<DataType>(type);
          status = m_in_ring.peek(m_data_size, sizeof(TOKEN_TYPE) << 1); // Quick 2x multiplication
          FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
          // Check the header for correctness
          if (start != START_WORD || m_data_type != COMMAND_TYPE || m_data_size >= MAX_DATA_SIZE) {
              m_in_ring.rotate(1);
              continue;
          }
          // Check for enough data to deserialize everything otherwise break and wait for more.
          else if (m_in_ring.get_remaining_size() < (HEADER_SIZE + m_data_size + sizeof(U32))) {
              break;
          }
          // Continue with the data portion and checksum
          m_in_ring.peek(checksum, HEADER_SIZE + m_data_size);
          // Check checksum
          if (checksum == END_WORD) {
              routeComData();
              m_in_ring.rotate(HEADER_SIZE + m_data_size + sizeof(U32));
          }
          // Failed checksum, keep looking for valid message
          else {
              m_in_ring.rotate(1);
          }
      }
  }

  void GroundInterfaceComponentImpl ::
    processBuffer(Fw::Buffer& buffer)
  {
      NATIVE_UINT_TYPE buffer_offset = 0;
      while (buffer_offset < buffer.getsize()) {
          NATIVE_UINT_TYPE ser_size = (buffer.getsize() >= m_in_ring.get_remaining_size(true)) ?
              m_in_ring.get_remaining_size(true) : static_cast<NATIVE_UINT_TYPE>(buffer.getsize());
          m_in_ring.serialize(reinterpret_cast<U8*>(buffer.getdata()) + buffer_offset, ser_size);
          buffer_offset = buffer_offset + ser_size;
          processRing();
      }
  }
} // end namespace Svc
