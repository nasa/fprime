// ====================================================================== 
// \title  GpsComponentImpl.hpp
// \author lemstarch
// \brief  hpp header file for the sample F' GPS component, based on a
//         NMEA GPS receiver.
//
// \copyright
// Copyright 2018, lestarch
// ====================================================================== 

#ifndef GpsComponentImpl_HPP
#define GpsComponentImpl_HPP

#include "GpsApp/Gps/GpsComponentAc.hpp"

// Need to define the memory footprint of our buffers. This means defining a count of buffers, and how big each is. In
// this example, we will allow the Gps component to manage its own buffers.
#define NUM_UART_BUFFERS 20
#define UART_READ_BUFF_SIZE 1024

namespace GpsApp {

  class GpsComponentImpl :
    public GpsComponentBase
  {
      /**
       * GpsPacket:
       *   A structure containing the information in the GPS location packet
       * received via the NMEA GPS receiver.
       */
      struct GpsPacket {
          float utcTime;
          float dmNS;
          char northSouth;
          float dmEW;
          char eastWest;
          unsigned int lock;
          unsigned int count;
          float filler;
          float altitude;
      };
    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Gps
      //!
      GpsComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Gps
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Preamble
      //!
      void preamble(void);

      //! Destroy object Gps
      //!
      ~GpsComponentImpl(void);

    PRIVATE:
      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for serialRecv
      //!
      void serialRecv_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer, /*!< Buffer containing data*/
          Drv::SerialReadStatus &serial_status /*!< Status of read*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations 
      // ----------------------------------------------------------------------

      //! Implementation for Gps_ReportLockStatus command handler
      //! A command to force an EVR reporting lock status.
      void Gps_ReportLockStatus_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );
      //!< Has the device acquired GPS lock?
      bool m_locked;
      //!< Create member variables to store buffers and the data array that those buffers use for storage
      Fw::Buffer m_recvBuffers[NUM_UART_BUFFERS];
      BYTE m_uartBuffers[NUM_UART_BUFFERS][UART_READ_BUFF_SIZE];
    };

} // end namespace GpsApp

#endif
