// ====================================================================== 
// \title  RpiDemoImpl.cpp
// \author tcanham
// \brief  cpp file for RpiDemo component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 


#include <RPI/RpiDemo/RpiDemoComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <RPI/Top/RpiSchedContexts.hpp>
#include <ctype.h>

namespace Rpi {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  RpiDemoComponentImpl ::
#if FW_OBJECT_NAMES == 1
    RpiDemoComponentImpl(
        const char *const compName
    ) :
      RpiDemoComponentBase(compName)
#else
    RpiDemoImpl(void)
#endif
    ,m_uartWriteBytes(0)
    ,m_uartReadBytes(0)
    ,m_spiBytes(0)
    ,m_currLedVal(GPIO_OUT_CLEAR)
  {


  }

  void RpiDemoComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    RpiDemoComponentBase::init(queueDepth, instance);
  }

  RpiDemoComponentImpl ::
    ~RpiDemoComponentImpl(void)
  {

  }

  void RpiDemoComponentImpl::preamble(void) {
      // send buffers to UART driver
      for (NATIVE_INT_TYPE buffer = 0; buffer < NUM_RPI_UART_BUFFERS; buffer++) {
          // assign buffers to buffer containers
          this->m_recvBuffers[buffer].setdata((U64)this->m_uartBuffers[buffer]);
          this->m_recvBuffers[buffer].setsize(RPI_UART_READ_BUFF_SIZE);
          this->UartBuffers_out(0,this->m_recvBuffers[buffer]);
      }
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void RpiDemoComponentImpl ::
    Run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      // check which rate group call it is
      switch (context) {
          case Rpi::CONTEXT_RPI_DEMO_1Hz:
              // write telemetry channels
              this->tlmWrite_RD_LastMsg(this->m_lastUartMsg);
              this->tlmWrite_RD_UartRecvBytes(this->m_uartReadBytes);
              this->tlmWrite_RD_UartSentBytes(this->m_uartWriteBytes);
              this->tlmWrite_RD_SpiBytes(this->m_spiBytes);
              break;
          case Rpi::CONTEXT_RPI_DEMO_10Hz:
              // Toggle LED value
              this->GpioWrite_out(0,(this->m_currLedVal == GPIO_OUT_SET)?true:false);
              this->m_currLedVal = (this->m_currLedVal == GPIO_OUT_SET)?GPIO_OUT_CLEAR:GPIO_OUT_SET;
              break;
          default:
              FW_ASSERT(0,context);
              break; // for the code checkers
      }
  }

  void RpiDemoComponentImpl ::
    UartRead_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer,
        Drv::SerialReadStatus &status
    )
  {
      // convert incoming data to string. If it is not printable, set character to '*'
      char uMsg[serBuffer.getsize()];
      char* bPtr = (char*)serBuffer.getdata();

      for (NATIVE_UINT_TYPE byte = 0; byte < sizeof(uMsg); byte++) {
          uMsg[byte] = isalpha(bPtr[byte])?bPtr[byte]:'*';
      }
      uMsg[sizeof(uMsg)-1] = 0;

      Fw::LogStringArg evrMsg(uMsg);
      this->log_ACTIVITY_HI_RD_UartMsgIn(evrMsg);
      this->m_lastUartMsg = uMsg;

      // reset buffer size
      serBuffer.setsize(RPI_UART_READ_BUFF_SIZE);
      // return buffer to driver
      this->UartBuffers_out(0,serBuffer);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void RpiDemoComponentImpl ::
    RD_SendString_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& text
    )
  {
      // Copy string to UART buffer

  }

  void RpiDemoComponentImpl ::
    RD_SetGpio_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 output,
        GpioOutVal value
    )
  {
      // set value of GPIO

  }

  void RpiDemoComponentImpl ::
    RD_GetGpio_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 output,
        GpioInVal value
    )
  {
      // get value of GPIO input
  }

  void RpiDemoComponentImpl ::
    RD_SendSpi_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& data
    )
  {
      // copy data from string to output buffer
  }

} // end namespace Rpi
