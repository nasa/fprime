// ======================================================================
// \title  GpsComponentImpl.cpp
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
      // Initialize the lock to "false"
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

  //Step 0: The linux serial driver keeps its storage externally. This means that we need to supply it some buffers to
  //        work with. This code will loop through our member variables holding the buffers and send them to the linux
  //        serial driver.  'preamble' is automatically called after the system is constructed, before the system runs
  //        at steady-state. This allows for initialization code that invokes working ports.
  void GpsComponentImpl :: preamble(void)
  {
      for (NATIVE_INT_TYPE buffer = 0; buffer < NUM_UART_BUFFERS; buffer++) {
          //Assign the raw data to the buffer. Make sure to include the side of the region assigned.
          this->m_recvBuffers[buffer].setdata((U64)this->m_uartBuffers[buffer]);
          this->m_recvBuffers[buffer].setsize(UART_READ_BUFF_SIZE);
          // Invoke the port to send the buffer out.
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

  // Step 1: serialIn
  //
  // By implementing this "handler" we can respond to the serial device sending us data buffers containing the GPS
  // data. This handles our serial messages. It should perform the actions we expect from the design phases.
  void GpsComponentImpl ::
    serialRecv_handler(
        const NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::Buffer &serBuffer, /*!< Buffer containing data*/
        Drv::SerialReadStatus &serial_status /*!< Status of read*/
    )
  {
      // Local variable definitions
      int status = 0;
      float lat = 0.0f, lon = 0.0f;
      GpsPacket packet;
      // Grab the size (used amount of the buffer) and a pointer to the data in the buffer
      U32 buffsize = static_cast<U32>(serBuffer.getsize());
      char* pointer = reinterpret_cast<char*>(serBuffer.getdata());
      // Check for invalid read status, log an error, return buffer and abort if there is a problem
      if (serial_status != Drv::SER_OK) {
          Fw::Logger::logMsg("[WARNING] Received buffer with bad packet: %d\n", serial_status);
          // We MUST return the buffer or the serial driver won't be able to reuse it. The same buffer send call is used
          // as we did in "preamble".  Since the buffer's size was overwritten to hold the actual data size, we need to
          // reset it to the full data block size before returning it.
          serBuffer.setsize(UART_READ_BUFF_SIZE);
          this->serialBufferOut_out(0, serBuffer);
          return;
      }
      // If not enough data is available for a full messsage, return the buffer and abort.
      else if (buffsize < 24) {
          // We MUST return the buffer or the serial driver won't be able to reuse it. The same buffer send call is used
          // as we did in "preamble".  Since the buffer's size was overwritten to hold the actual data size, we need to
          // reset it to the full data block size before returning it.
          serBuffer.setsize(UART_READ_BUFF_SIZE);
          this->serialBufferOut_out(0, serBuffer);
          return;
      }
      //Step 2:
      //  Parse the GPS message from the UART (looking for $GPGGA messages). This uses standard C functions to read all
      //  the defined protocol messages into our GPS package struct. If all 9 items are parsed, we break. Otherwise we
      //  continue to scan the block of data looking for messages further in.
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
      //If we failed to find the GPGGA then return the buffer and abort.
      if (status == 0) {
          // We MUST return the buffer or the serial driver won't be able to reuse it. The same buffer send call is used
          // as we did in "preamble".  Since the buffer's size was overwritten to hold the actual data size, we need to
          // reset it to the full data block size before returning it.
          serBuffer.setsize(UART_READ_BUFF_SIZE);
          this->serialBufferOut_out(0, serBuffer);
          return;
      }
      // If we found some of the message but not all of the message, then log an error, return the buffer and exit.
      else if (status != 9) {
          Fw::Logger::logMsg("[ERROR] GPS parsing failed: %d\n", status);
          // We MUST return the buffer or the serial driver won't be able to reuse it. The same buffer send call is used
          // as we did in "preamble".  Since the buffer's size was overwritten to hold the actual data size, we need to
          // reset it to the full data block size before returning it.
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
      //Step 4: call the downlink functions to send down data
      tlmWrite_Gps_Latitude(lat);
      tlmWrite_Gps_Longitude(lon);
      tlmWrite_Gps_Altitude(packet.altitude);
      tlmWrite_Gps_Count(packet.count);
      //Lock status update only if changed
      //Step 5,6: note changed lock status
      // Emit an event if the lock has been acquired, or lost
      if (packet.lock == 0 && m_locked) {
          m_locked = false;
          log_WARNING_HI_Gps_LockLost();
      } else if (packet.lock == 1 && !m_locked) {
          m_locked = true;
          log_ACTIVITY_HI_Gps_LockAquired();
      }
      // We MUST return the buffer or the serial driver won't be able to reuse it. The same buffer send call is used
      // as we did in "preamble".  Since the buffer's size was overwritten to hold the actual data size, we need to
      // reset it to the full data block size before returning it.
      serBuffer.setsize(UART_READ_BUFF_SIZE);
      this->serialBufferOut_out(0, serBuffer);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------
  //Step 7,8: respond to a command to report lock status.
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
    //Step 9: complete command
    this->cmdResponse_out(opCode, cmdSeq, Fw::COMMAND_OK);
  }
} // end namespace GpsApp
