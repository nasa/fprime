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
#include <FpConfig.hpp>
#include <ctype.h>

namespace RPI {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  RpiDemoComponentImpl ::
    RpiDemoComponentImpl(
        const char *const compName
    ) :
      RpiDemoComponentBase(compName)
    ,m_uartWriteBytes(0)
    ,m_uartReadBytes(0)
    ,m_spiBytes(0)
    ,m_currLedVal(Fw::Logic::LOW)
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
    ~RpiDemoComponentImpl()
  {

  }

  void RpiDemoComponentImpl::preamble() {
      // check initial state parameter
      Fw::ParamValid valid;
      RpiDemo_LedState initState = paramGet_RD_PrmLedInitState(valid);
      // check status
      switch (valid.e) {
          // if default or valid, use stored value
          case Fw::ParamValid::DEFAULT:
          case Fw::ParamValid::VALID:
              this->m_ledOn = (RpiDemo_LedState::BLINKING == initState.e);
              this->log_ACTIVITY_HI_RD_LedBlinkState(
                  this->m_ledOn ?
                  RpiDemo_LedState::BLINKING : RpiDemo_LedState::OFF
              );
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
          case RG_CONTEXT_1Hz:
              // write telemetry channels
              this->tlmWrite_RD_LastMsg(this->m_lastUartMsg);
              this->tlmWrite_RD_UartRecvBytes(this->m_uartReadBytes);
              this->tlmWrite_RD_UartSentBytes(this->m_uartWriteBytes);
              this->tlmWrite_RD_SpiBytes(this->m_spiBytes);
              this->tlmWrite_RD_1HzTicks(this->m_1HzTicks);
              this->tlmWrite_RD_10HzTicks(this->m_10HzTicks);
              this->m_1HzTicks++;
              break;
          case RG_CONTEXT_10Hz:
              // Toggle LED value
              if ( (this->m_10HzTicks++%this->m_ledDivider == 0) and this->m_ledOn) {
                  this->GpioWrite_out(2, this->m_currLedVal);
                  this->m_currLedVal = (this->m_currLedVal == Fw::Logic::HIGH) ?
                    Fw::Logic::LOW : Fw::Logic::HIGH;
              }
              break;
          default:
              FW_ASSERT(0, context);
              break; // for the code checkers
      }

  }

  void RpiDemoComponentImpl ::
    UartRead_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer,
        const Drv::RecvStatus &status
    )
  {
      if (Drv::RecvStatus::RECV_OK == status.e) {
          // convert incoming data to string. If it is not printable, set character to '*'
          char uMsg[serBuffer.getSize() + 1];
          char *bPtr = reinterpret_cast<char *>(serBuffer.getData());

          for (NATIVE_UINT_TYPE byte = 0; byte < serBuffer.getSize(); byte++) {
            uMsg[byte] = isalpha(bPtr[byte]) ? bPtr[byte] : '*';
          }
          uMsg[sizeof(uMsg) - 1] = 0;

          Fw::LogStringArg evrMsg(uMsg);
          this->log_ACTIVITY_HI_RD_UartMsgIn(evrMsg);
          this->m_lastUartMsg = uMsg;
          this->m_uartReadBytes += serBuffer.getSize();
      }
      // return buffer to buffer manager
      this->UartBuffers_out(0, serBuffer);
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
      txt.setSize(text.length());
      txt.setData(reinterpret_cast<U8*>(const_cast<char*>(text.toChar())));
      Drv::SendStatus status = this->UartWrite_out(0, txt);
      if (Drv::SendStatus::SEND_OK == status.e) {
        this->m_uartWriteBytes += text.length();

        Fw::LogStringArg arg = text;
        this->log_ACTIVITY_HI_RD_UartMsgOut(arg);
      }
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void RpiDemoComponentImpl ::
    RD_SetGpio_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        RpiDemo_GpioOutNum output, /*!< Output GPIO*/
        Fw::Logic value
    )
  {
      NATIVE_INT_TYPE port;
      // convert to connected ports
      switch (output.e) {
          case RpiDemo_GpioOutNum::PIN_23:
              port = 0;
              break;
          case RpiDemo_GpioOutNum::PIN_24:
              port = 1;
              break; // good values
          default: // bad values
              this->log_WARNING_HI_RD_InvalidGpio(output.e);
              this->cmdResponse_out(
                  opCode,
                  cmdSeq,
                  Fw::CmdResponse::VALIDATION_ERROR
              );
              return;
      }
      // set value of GPIO
      this->GpioWrite_out(port, value);
      this->log_ACTIVITY_HI_RD_GpioSetVal(output.e, value);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void RpiDemoComponentImpl ::
    RD_GetGpio_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        RpiDemo_GpioInNum input /*!< Input GPIO*/
    )
  {
      NATIVE_INT_TYPE port;
      // convert to connected ports
      switch (input.e) {
          case RpiDemo_GpioInNum::PIN_25:
              port = 0;
              break;
          case RpiDemo_GpioInNum::PIN_17:
              port = 1;
              break; // good values
          default: // bad values
              this->log_WARNING_HI_RD_InvalidGpio(input.e);
              this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
              return;
      }
      // get value of GPIO input
      Fw::Logic val;
      this->GpioRead_out(port, val);
      this->log_ACTIVITY_HI_RD_GpioGetVal(input.e, val);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
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
      in.setData(reinterpret_cast<U8*>(inBuf));
      in.setSize(sizeof(inBuf));

      Fw::Buffer out;
      out.setData(reinterpret_cast<U8*>(const_cast<char*>(data.toChar())));
      out.setSize(data.length());
      this->SpiReadWrite_out(0, out, in);
      for (NATIVE_UINT_TYPE byte = 0; byte < sizeof(inBuf); byte++) {
          inBuf[byte] = isalpha(inBuf[byte])?inBuf[byte]:'*';
      }
      inBuf[sizeof(inBuf)-1] = 0;
      // write reply to event
      Fw::LogStringArg arg = inBuf;
      this->log_ACTIVITY_HI_RD_SpiMsgIn(arg);
      this->m_spiBytes += data.length();

      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void RpiDemoComponentImpl ::
    RD_SetLed_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        RpiDemo_LedState value
    )
  {
      this->m_ledOn = (RpiDemo_LedState::BLINKING == value.e);
      this->log_ACTIVITY_HI_RD_LedBlinkState(
          this->m_ledOn ? RpiDemo_LedState::BLINKING : RpiDemo_LedState::OFF);
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
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
          this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
          return;
      }
      this->m_ledDivider = divider;
      this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }


} // end namespace RPI
