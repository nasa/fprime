// ====================================================================== 
// \title  GpsImpl.cpp
// \author lestarch
// \brief  cpp implementation of the F' sample GPS receiver for a
//         NMEA GPS receiver device.
//
// \copyright
// Copyright 2018, lestarch.
// ====================================================================== 

#include <GpsApp/Gps/GpsComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Logger/Logger.hpp"

//File path to UART device on UNIX system
#include <cstring>

namespace GpsApp {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  GpsComponentImpl ::
#if FW_OBJECT_NAMES == 1
    GpsComponentImpl(
        const char *const compName
    ) :
      GpsComponentBase(compName),
#else
      GpsComponentBase(void),
#endif
      m_locked(false)
  {

  }

  void GpsComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    GpsComponentBase::init(queueDepth, instance);
  }

  void GpsComponentImpl :: preamble(void)
  {
      // send buffers to UART driver
      for (NATIVE_INT_TYPE buffer = 0; buffer < NUM_UART_BUFFERS; buffer++) {
          // assign buffers to buffer containers
          this->m_recvBuffers[buffer].setdata((U64)this->m_uartBuffers[buffer]);
          this->m_recvBuffers[buffer].setsize(UART_READ_BUFF_SIZE);
          this->serialBufferOut_out(0, this->m_recvBuffers[buffer]);
      }
  }

  GpsComponentImpl ::
    ~GpsComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  // Step 0: serialIn
  //
  //  By implementing this "handler" we can respond to the 1HZ call allowing
  //  us to read the GPS UART every 1 second.
  void GpsComponentImpl ::
    serialRecv_handler(
        const NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &serBuffer, /*!< Buffer containing data*/
        Drv::SerialReadStatus &serial_status /*!< Status of read*/
    )
  {
      int status = 0;
      float lat = 0.0f, lon = 0.0f;
      GpsPacket packet;
      U32 buffsize = static_cast<U32>(serBuffer.getsize());
      char* pointer = reinterpret_cast<char*>(serBuffer.getdata());
      if (serial_status != Drv::SER_OK) {
          Fw::Logger::logMsg("[WARNING] Received buffer with bad packet: %d\n", serial_status);
          serBuffer.setsize(UART_READ_BUFF_SIZE);
          this->serialBufferOut_out(0, serBuffer);
          return;
      } else if (buffsize < 24) {
          serBuffer.setsize(UART_READ_BUFF_SIZE);
          this->serialBufferOut_out(0, serBuffer);
          return;
      }
      //Look for a recognized GPS location packet and parse it
      //Step 3:
      // Parse the GPS message from the UART (looking for $GPPA messages)
      for (U32 i = 0; i < (buffsize - 24); i++) {
          status = sscanf(pointer, "$GPGGA,%f,%f,%c,%f,%c,%u,%u,%f,%f",
              &packet.utcTime, &packet.dmNS, &packet.northSouth,
              &packet.dmEW, &packet.eastWest, &packet.lock,
              &packet.count, &packet.filler, &packet.altitude);
          //Break when all GPS items are found
          if (status == 9) {
              break;
          }
          pointer = pointer + 1;
      }
      //If we failed to find the GPGGA then exit. If failed to extract data then return
      if (status == 0) {
          serBuffer.setsize(UART_READ_BUFF_SIZE);
          this->serialBufferOut_out(0, serBuffer);
          return;
      } else if (status != 9) {
          Fw::Logger::logMsg("[ERROR] GPS parsing failed: %d\n", status);
          serBuffer.setsize(UART_READ_BUFF_SIZE);
          this->serialBufferOut_out(0, serBuffer);
          return;
      }
      //GPS packet locations are of the form: ddmm.mmmm
      //We will convert to lat/lon in degrees only before downlinking
      //Latitude degrees, add on minutes (converted to degrees), multiply by direction
      lat = (U32)(packet.dmNS/100.0f);
      lat = lat + (packet.dmNS - (lat * 100.0f))/60.0f;
      lat = lat * ((packet.northSouth == 'N') ? 1 : -1);
      //Longitude degrees, add on minutes (converted to degrees), multiply by direction
      lon = (U32)(packet.dmEW/100.0f);
      lon = lon + (packet.dmEW - (lon * 100.0f))/60.f;
      lon = lon * ((packet.eastWest == 'E') ? 1 : -1);
      //Step 4: downlink
      // Call the downlink functions to send down data
      Fw::Logger::logMsg("[INFO] Current lat, lon: (%f,%f)\n", (double)lat, (double)lon);
      tlmWrite_Gps_Latitude(lat);
      tlmWrite_Gps_Longitude(lon);
      tlmWrite_Gps_Altitude(packet.altitude);
      tlmWrite_Gps_Count(packet.count);
      //Lock status update only if changed
      //Step 7,8: note changed lock status
      // Emit an event if the lock has been aquired, or lost
      if (packet.lock == 0 && m_locked) {
          m_locked = false;
          log_WARNING_HI_Gps_LockLost();
      } else if (packet.lock == 1 && !m_locked) {
          m_locked = true;
          log_ACTIVITY_HI_Gps_LockAquired();
      }
      serBuffer.setsize(UART_READ_BUFF_SIZE);
      this->serialBufferOut_out(0, serBuffer);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------
  //Step 9: respond to status command
  //
  // When a status command is received, respond by emitting the 
  // current lock status as an Event.
  void GpsComponentImpl ::
    Gps_ReportLockStatus_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    //Locked-force print
    if (m_locked) {
        log_ACTIVITY_HI_Gps_LockAquired();
    } else {
        log_WARNING_HI_Gps_LockLost();
    }
  }
} // end namespace GpsApp
