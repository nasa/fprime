// ====================================================================== 
// \title  UdpReceiverImpl.hpp
// \author tcanham
// \brief  hpp file for UdpReceiver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef UdpReceiver_HPP
#define UdpReceiver_HPP

#include "Svc/UdpReceiver/UdpReceiverComponentAc.hpp"
#include "Svc/UdpReceiver/UdpReceiverComponentImplCfg.hpp"

namespace Svc {

  class UdpReceiverComponentImpl :
    public UdpReceiverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object UdpReceiver
      //!
      UdpReceiverComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object UdpReceiver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object UdpReceiver
      //!
      ~UdpReceiverComponentImpl(void);

      //! Open the connection
      void open(
              const char* port
              );

      //! start worker thread
      void startThread(
              NATIVE_UINT_TYPE priority, /*!< read task priority */
              NATIVE_UINT_TYPE stackSize, /*!< stack size */
              NATIVE_UINT_TYPE affinity /*!< cpu affinity */
      );


    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for Sched
      //!
      void Sched_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

      static void workerTask(void* ptr); //!< worker task entry point
      void doRecv(void); //!< receives a single packet (helps unit testing)
      Os::Task m_socketTask;

      NATIVE_INT_TYPE m_fd; //!< socket file descriptor

      class UdpSerialBuffer :
        public Fw::SerializeBufferBase
      {

        public:

#ifdef BUILD_UT
          void operator=(const UdpSerialBuffer& other);
          UdpSerialBuffer(const Fw::SerializeBufferBase& other);
          UdpSerialBuffer(const UdpSerialBuffer& other);
          UdpSerialBuffer();
#endif

          NATIVE_UINT_TYPE getBuffCapacity(void) const {
            return sizeof(m_buff);
          }

          // Get the max number of bytes that can be serialized
          NATIVE_UINT_TYPE getBuffSerLeft(void) const {

            const NATIVE_UINT_TYPE size  = getBuffCapacity();
            const NATIVE_UINT_TYPE loc = getBuffLength();

            if (loc >= (size-1) ) {
                return 0;
            }
            else {
                return (size - loc - 1);
            }
          }

          U8* getBuffAddr(void) {
            return m_buff;
          }

          const U8* getBuffAddr(void) const {
            return m_buff;
          }

        private:
          // Should be the max of all the input ports serialized sizes...
          U8 m_buff[UDP_RECEIVER_MSG_SIZE];

      } m_recvBuff;

      UdpSerialBuffer m_portBuff; //!< working buffer for decoding packets

      U32 m_packetsReceived; //!< number of packets received
      U32 m_bytesReceived; //!< number of bytes received
      U32 m_packetsDropped; //!< number of detected packets dropped
      U32 m_decodeErrors; //!< deserialization errors

      bool m_firstSeq; //!< first sequence number detected
      U8 m_currSeq; //!< current tracked sequence number


    };

} // end namespace Svc

#endif
