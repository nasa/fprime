// ====================================================================== 
// \title  RpiDemoImpl.cpp
// \author tcanham
// \brief  cpp file for RpiDemo component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
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
    ,m_ledOn(true)
    ,m_ledDivider(10) // start at 1Hz
    ,m_1HzTicks(0)
    ,m_10HzTicks(0)
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
      // check initial state parameter
      Fw::ParamValid valid;
      LedStatePrm initState = paramGet_RD_PrmLedInitState(valid);
      // check status
      switch (valid) {
          // if default or valid, use stored value
          case Fw::PARAM_DEFAULT:
          case Fw::PARAM_VALID:
              this->m_ledOn = (LED_STATE_BLINKING_PRM == initState)?true:false;
              this->log_ACTIVITY_HI_RD_LedBlinkState(this->m_ledOn?LED_STATE_BLINKING_EV:LED_STATE_OFF_EV);
              break;
          default:
              // use constructor default
              break;
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
              this->tlmWrite_RD_1HzTicks(this->m_1HzTicks);
              this->tlmWrite_RD_10HzTicks(this->m_10HzTicks);
              this->m_1HzTicks++;
              break;
          case Rpi::CONTEXT_RPI_DEMO_10Hz:
              // Toggle LED value
              if ( (this->m_10HzTicks++%this->m_ledDivider == 0) and this->m_ledOn) {
                  this->GpioWrite_out(2,(this->m_currLedVal == GPIO_OUT_SET)?true:false);
                  this->m_currLedVal = (this->m_currLedVal == GPIO_OUT_SET)?GPIO_OUT_CLEAR:GPIO_OUT_SET;
              }
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
      char uMsg[serBuffer.getsize()+1];
      char* bPtr = (char*)serBuffer.getdata();

      for (NATIVE_UINT_TYPE byte = 0; byte < serBuffer.getsize(); byte++) {
          uMsg[byte] = isalpha(bPtr[byte])?bPtr[byte]:'*';
      }
      uMsg[sizeof(uMsg)-1] = 0;

      Fw::LogStringArg evrMsg(uMsg);
      this->log_ACTIVITY_HI_RD_UartMsgIn(evrMsg);
      this->m_lastUartMsg = uMsg;
      this->m_uartReadBytes += serBuffer.getsize();

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
      Fw::Buffer txt;
      txt.setsize(text.length());
      txt.setdata((U64)text.toChar());
      this->UartWrite_out(0,txt);
      this->m_uartWriteBytes += text.length();
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void RpiDemoComponentImpl ::
    RD_SetGpio_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        GpioOutNum output, /*!< Output GPIO*/
        GpioOutVal value
    )
  {
      NATIVE_INT_TYPE port;
      // convert to connected ports
      switch (output) {
          case GPIO_OUT_23:
              port = 0;
              break;
          case GPIO_OUT_24:
              port = 1;
              break; // good values
          default: // bad values
              this->log_WARNING_HI_RD_InvalidGpio(output);
              this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
              return;
      }
      // set value of GPIO
      this->GpioWrite_out(port,GPIO_OUT_SET == value?true:false);
      this->log_ACTIVITY_HI_RD_GpioSetVal(output,GPIO_OUT_SET == value?GPIO_OUT_SET_EV:GPIO_OUT_CLEAR_EV);
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void RpiDemoComponentImpl ::
    RD_GetGpio_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        GpioInNum input /*!< Input GPIO*/
    )
  {
      NATIVE_INT_TYPE port;
      // convert to connected ports
      switch (input) {
          case GPIO_IN_25:
              port = 0;
              break;
          case GPIO_IN_17:
              port = 1;
              break; // good values
          default: // bad values
              this->log_WARNING_HI_RD_InvalidGpio(input);
              this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
              return;
      }
      // get value of GPIO input
      bool val;
      this->GpioRead_out(port,val);
      this->log_ACTIVITY_HI_RD_GpioGetVal(input,val?GPIO_IN_SET_EV:GPIO_IN_CLEAR_EV);
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void RpiDemoComponentImpl ::
    RD_SendSpi_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& data
    )
  {
      // copy data from string to output buffer
      char inBuf[data.length()+1];
      Fw::Buffer in;
      in.setdata((U64)inBuf);
      in.setsize(sizeof(inBuf));

      Fw::Buffer out;
      out.setdata((U64)data.toChar());
      out.setsize(data.length());
      this->SpiReadWrite_out(0,out,in);
      for (NATIVE_UINT_TYPE byte = 0; byte < sizeof(inBuf); byte++) {
          inBuf[byte] = isalpha(inBuf[byte])?inBuf[byte]:'*';
      }
      inBuf[sizeof(inBuf)-1] = 0;
      // write reply to event
      Fw::LogStringArg arg = inBuf;
      this->log_ACTIVITY_HI_RD_SpiMsgIn(arg);
      this->m_spiBytes += data.length();

      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void RpiDemoComponentImpl ::
    RD_SetLed_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        LedState value
    )
  {
      this->m_ledOn = LED_STATE_BLINKING == value?true:false;
      this->log_ACTIVITY_HI_RD_LedBlinkState(this->m_ledOn?LED_STATE_BLINKING_EV:LED_STATE_OFF_EV);
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void RpiDemoComponentImpl ::
    RD_SetLedDivider_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 divider
    )
  {
      if (divider < 1) {
          this->log_WARNING_HI_RD_InvalidDivider(divider);
          this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_VALIDATION_ERROR);
          return;
      }
      this->m_ledDivider = divider;
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }


} // end namespace Rpi
