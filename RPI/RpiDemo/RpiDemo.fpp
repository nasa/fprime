module RPI {

  @ An RPI demonstrator component
  active component RpiDemo {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------

    enum GpioInNum {
      PIN_25 = 0
      PIN_17 = 1
    }

    enum GpioOutNum {
      PIN_23 = 0
      PIN_24 = 1
    }

    enum LedState {
      BLINKING = 0
      OFF = 1
    }

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Input Scheduler port - called at 1Hz and 10Hz
    async input port Run: Svc.Sched

    @ Input port for receiving UART data
    async input port UartRead: Drv.ByteStreamRecv

    @ Output Port for reading GPIO values
    output port GpioRead: [2] Drv.GpioRead

    @ Output Port for reading/writing SPI
    output port SpiReadWrite: Drv.SpiReadWrite

    @ Output Port for writing GPIO values
    output port GpioWrite: [3] Drv.GpioWrite

    @ Output Port for writing UART data
    output port UartWrite: Drv.ByteStreamSend

    @ Output port for sending UART buffers to use for reading
    output port UartBuffers: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive
    command recv port cmdIn

    @ Command registration
    command reg port cmdRegOut

    @ Command response
    command resp port cmdResponseOut

    @ Event
    event port eventOut

    @ Parameter get
    param get port prmGetOut

    @ Parameter set
    param set port prmSetOut

    @ Telemetry
    telemetry port tlmOut

    @ Text event
    text event port textEventOut

    @ Time get
    time get port timeGetOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Command to send a string to the UART
    async command RD_SendString(
                                 $text: string size 40 @< String to send
                               ) \
      opcode 0

    @ Sets LED state
    async command RD_SetLed(
                             value: LedState @< GPIO value
                           ) \
      opcode 1

    @ Sets the divided rate of the LED
    async command RD_SetLedDivider(
                                    divider: U32 @< Divide 10Hz by this number
                                  ) \
      opcode 2

    @ Sets a GPIO port value
    async command RD_SetGpio(
                              $output: GpioOutNum @< Output GPIO
                              value: Fw.Logic @< GPIO value
                            ) \
      opcode 3

    @ Gets a GPIO port value
    async command RD_GetGpio(
                              $input: GpioInNum @< Input GPIO
                            ) \
      opcode 4

    @ Sends SPI data, prints read data
    async command RD_SendSpi(
                              data: string size 40 @< data to send
                            ) \
      opcode 5

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Message sent on UART
    event RD_UartMsgOut(
                         msg: string size 40 @< The message
                       ) \
      severity activity high \
      id 0 \
      format "Sent msg {} on UART"

    @ Message received on UART
    event RD_UartMsgIn(
                        msg: string size 40 @< The message
                      ) \
      severity activity high \
      id 1 \
      format "Received msg {} on UART"

    @ GPIO set
    event RD_GpioSetVal(
                         $output: U32 @< The output number
                         value: Fw.Logic @< GPIO value
                       ) \
      severity activity high \
      id 2 \
      format "GPIO {} set to {}"

    @ GPIO get
    event RD_GpioGetVal(
                         $output: U32 @< The output number
                         value: Fw.Logic @< GPIO value
                       ) \
      severity activity high \
      id 3 \
      format "GPIO {} read as {}"

    @ Message received on SPI
    event RD_SpiMsgIn(
                       msg: string size 40 @< The message bytes as text
                     ) \
      severity activity high \
      id 4 \
      format "Received msg {} on SPI"

    @ Invalid GPIO requested
    event RD_InvalidGpio(
                          val: U32 @< The bad GPIO number
                        ) \
      severity warning high \
      id 5 \
      format "Invalid GPIO {} requested"

    @ Message received on UART
    event RD_InvalidDivider(
                             val: U32 @< The bad divider value
                           ) \
      severity warning high \
      id 6 \
      format "Invalid divider {} requested"

    # ----------------------------------------------------------------------
    # Parameters
    # ----------------------------------------------------------------------

    @ Blink state set to new value
    event RD_LedBlinkState(
                            val: LedState @< The blink state
                          ) \
      severity activity high \
      id 7 \
      format "LED blink state set to {}"

    @ Parameter to determine initial state of blinking LED
    param RD_PrmLedInitState: LedState default LedState.BLINKING id 0 \
      set opcode 10 \
      save opcode 11

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ Number of bytes sent on the UART
    telemetry RD_UartSentBytes: U32 id 0

    @ Number of bytes sent on the UART
    telemetry RD_UartRecvBytes: U32 id 1

    @ Last received UART text message
    telemetry RD_LastMsg: string size 40 id 2

    @ Number of bytes sent on SPI
    telemetry RD_SpiBytes: U32 id 3

    @ Ticks at 1Hz
    telemetry RD_1HzTicks: U32 id 4

    @ Ticks at 10Hz
    telemetry RD_10HzTicks: U32 id 5

  }

}
