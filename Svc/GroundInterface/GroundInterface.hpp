// ======================================================================
// \title  GroundInterfaceImpl.hpp
// \author lestarch
// \brief  hpp file for GroundInterface component implementation class
// ======================================================================
#include <Fw/Types/Serializable.hpp>
#include "Svc/GroundInterface/GroundInterfaceComponentAc.hpp"
#include "Utils/Types/CircularBuffer.hpp"
#ifndef GroundInterface_HPP
#define GroundInterface_HPP

#define GND_BUFFER_SIZE 1024
#define TOKEN_TYPE U32
#define HEADER_SIZE (2 * sizeof(TOKEN_TYPE))

namespace Svc {

  class GroundInterfaceComponentImpl :
    public GroundInterfaceComponentBase
  {
    public:
      static const U32 MAX_DATA_SIZE;
      static const TOKEN_TYPE START_WORD;
      static const U32 END_WORD;
      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object GroundInterface
      //!
      GroundInterfaceComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object GroundInterface
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object GroundInterface
      //!
      ~GroundInterfaceComponentImpl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for downlinkPort
      //!
      void downlinkPort_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::ComBuffer &data, /*!< Buffer containing packet data*/
          U32 context /*!< Call context value; meaning chosen by user*/
      );

      //! Handler implementation for fileDownlinkBufferSendIn
      //!
      void fileDownlinkBufferSendIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

      //! Handler implementation for readCallback
      //!
      void readCallback_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &fwBuffer
      );

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 context /*!< The call order*/
      );
      //! Frame and send some data
      //!
      void frame_send(
          U8* data, /*!< Data to be framed and sent out */
          TOKEN_TYPE size, /*!< Size of data in typed format */
          TOKEN_TYPE packet_type = Fw::ComPacket::FW_PACKET_UNKNOWN /*!< Packet type override for anonymous data i.e. file downlink */
      );

      //! Processes the out-going data into coms order
      void routeComData();

      //! Process all the data in the ring
      void processRing();

      //! Process a data buffer containing a read from the serial port
      void processBuffer(Fw::Buffer& data /*!< Data to process */);

      // Basic data movement variables
      Fw::Buffer m_ext_buffer;
      U8 m_buffer[GND_BUFFER_SIZE];
      // Input variables
      TOKEN_TYPE m_data_size; //!< Data size expected in incoming data
      U8 m_in_buffer[GND_BUFFER_SIZE];
      Types::CircularBuffer m_in_ring;
    };

} // end namespace Svc

#endif
