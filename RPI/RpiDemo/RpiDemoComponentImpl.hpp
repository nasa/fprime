// ====================================================================== 
// \title  RpiDemoImpl.hpp
// \author tcanham
// \brief  hpp file for RpiDemo component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef RpiDemo_HPP
#define RpiDemo_HPP

#include "RPI/RpiDemo/RpiDemoComponentAc.hpp"
#include <RPI/RpiDemo/RpiDemoComponentImplCfg.hpp>

namespace Rpi {

  class RpiDemoComponentImpl :
    public RpiDemoComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object RpiDemo
      //!
      RpiDemoComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object RpiDemo
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object RpiDemo
      //!
      ~RpiDemoComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for Run
      //!
      void Run_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

      //! Handler implementation for UartRead
      //!
      void UartRead_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer &serBuffer, /*!< Buffer containing data*/
          Drv::SerialReadStatus &status /*!< Status of read*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations 
      // ----------------------------------------------------------------------

      //! Implementation for RD_SendString command handler
      //! Command to send a string to the UART
      void RD_SendString_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& text /*!< String to send*/
      );

      //! Implementation for RD_SetGpio command handler
      //! Sets a GPIO port value
      void RD_SetGpio_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          GpioOutNum output, /*!< Output GPIO*/
          GpioOutVal value /*!< GPIO value*/
      );

      //! Implementation for RD_GetGpio command handler
      //! Gets a GPIO port value
      void RD_GetGpio_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          GpioInNum input /*!< Input GPIO*/
      );

      //! Implementation for RD_SendSpi command handler
      //! Sends SPI data, prints read data
      void RD_SendSpi_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& data /*!< data to send*/
      );

      //! Implementation for RD_SetLed command handler
      //! Sets LED state
      void RD_SetLed_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          LedState value /*!< GPIO value*/
      );

      //! Implementation for RD_SetLedDivider command handler
      //! Sets the divided rate of the LED
      void RD_SetLedDivider_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U32 divider /*!< Divide 10Hz by this number*/
      );


      // This will be called once when task starts up
      void preamble(void);

      // telemetry values
      U32 m_uartWriteBytes;
      U32 m_uartReadBytes;
      U32 m_spiBytes;
      Fw::TlmString m_lastUartMsg;
      GpioOutVal m_currLedVal;
      // serial buffers
      Fw::Buffer m_recvBuffers[NUM_RPI_UART_BUFFERS];
      BYTE m_uartBuffers[NUM_RPI_UART_BUFFERS][RPI_UART_READ_BUFF_SIZE];
      // LED enabled
      bool m_ledOn;
      // toggle LED divider
      U32 m_ledDivider;
      // 10Hz ticks
      U32 m_1HzTicks;
      // 10Hz ticks
      U32 m_10HzTicks;

    };

} // end namespace Rpi

#endif
