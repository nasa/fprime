// ======================================================================
// \title  UdpSenderImpl.hpp
// \author tcanham
// \brief  hpp file for UdpSender component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UdpSender_HPP
#define UdpSender_HPP

#include <Svc/UdpSender/UdpSenderComponentAc.hpp>
#include <UdpSenderComponentImplCfg.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>


namespace Svc {

  class UdpSenderComponentImpl :
    public UdpSenderComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object UdpSender
      //!
      UdpSenderComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object UdpSender
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE msgSize, /*!< The message size*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );


      //! Destroy object UdpSender
      //!
      ~UdpSenderComponentImpl();

      //! Open the connection
      //!
      void open(
              const char* addr,  /*!< server address */
              const char* port /*!< server port */
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for Sched
      //!
      void Sched_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined serial input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for PortsIn
      //!
      void PortsIn_handler(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
      );

      NATIVE_INT_TYPE m_fd; //!< file descriptor for UDP socket
      NATIVE_UINT_TYPE m_packetsSent; //!< number of packets sent to server
      NATIVE_UINT_TYPE m_bytesSent; //!< number of bytes sent to server
      U8 m_seq; //!< packet sequence number; used to detect drops on the server end

      struct sockaddr_in m_servAddr;  //!< server address for sends

      class UdpSerialBuffer :
        public Fw::SerializeBufferBase
      {

        public:

#ifdef BUILD_UT
          UdpSerialBuffer& operator=(const UdpSerialBuffer& other);
          UdpSerialBuffer(const Fw::SerializeBufferBase& other);
          UdpSerialBuffer(const UdpSerialBuffer& other);
          UdpSerialBuffer();
#endif

          NATIVE_UINT_TYPE getBuffCapacity() const {
            return sizeof(m_buff);
          }

          // Get the max number of bytes that can be serialized
          NATIVE_UINT_TYPE getBuffSerLeft() const {

            const NATIVE_UINT_TYPE size  = getBuffCapacity();
            const NATIVE_UINT_TYPE loc = getBuffLength();

            if (loc >= (size-1) ) {
                return 0;
            }
            else {
                return (size - loc - 1);
            }
          }

          U8* getBuffAddr() {
            return m_buff;
          }

          const U8* getBuffAddr() const {
            return m_buff;
          }

        private:
          // Should be the max of all the input ports serialized sizes...
          U8 m_buff[UDP_SENDER_MSG_SIZE];

      } m_sendBuff;

    };

} // end namespace Svc

#endif
