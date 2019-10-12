<title>RpiDemo Component Dictionary</title>
# RpiDemo Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|RD_SendString|0 (0x0)|Command to send a string to the UART| | |
| | | |text|Fw::CmdStringArg|String to send|
|RD_SetLed|1 (0x1)|Sets LED state| | |
| | | |value|LedState|GPIO value|
|RD_SetLedDivider|2 (0x2)|Sets the divided rate of the LED| | |
| | | |divider|U32|Divide 10Hz by this number|
|RD_SetGpio|3 (0x3)|Sets a GPIO port value| | |
| | | |output|GpioOutNum|Output GPIO|
| | | |value|GpioOutVal|GPIO value|
|RD_GetGpio|4 (0x4)|Gets a GPIO port value| | |
| | | |input|GpioInNum|Input GPIO|
|RD_SendSpi|5 (0x5)|Sends SPI data, prints read data| | |
| | | |data|Fw::CmdStringArg|data to send|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|RD_UartSentBytes|0 (0x0)|U32|Number of bytes sent on the UART|
|RD_UartRecvBytes|1 (0x1)|U32|Number of bytes sent on the UART|
|RD_LastMsg|2 (0x2)|string|Last received UART text message|
|RD_SpiBytes|3 (0x3)|U32|Number of bytes sent on SPI|
|RD_1HzTicks|4 (0x4)|U32|Ticks at 1Hz|
|RD_10HzTicks|5 (0x5)|U32|Ticks at 10Hz|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|RD_UartMsgOut|0 (0x0)|Message sent on UART| | | | |
| | | |msg|Fw::LogStringArg&|40|The message|
|RD_UartMsgIn|1 (0x1)|Message received on UART| | | | |
| | | |msg|Fw::LogStringArg&|40|The message|
|RD_GpioSetVal|2 (0x2)|GPIO set| | | | |
| | | |output|U32||The output number|
| | | |value|GpioOutValEv||GPIO value|
|RD_GpioGetVal|3 (0x3)|GPIO get| | | | |
| | | |output|U32||The output number|
| | | |value|GpioInValEv||GPIO value|
|RD_SpiMsgIn|4 (0x4)|Message received on SPI| | | | |
| | | |msg|Fw::LogStringArg&|40|The message bytes as text|
|RD_InvalidGpio|5 (0x5)|Invalid GPIO requested| | | | |
| | | |val|U32||The bad GPIO number|
|RD_InvalidDivider|6 (0x6)|Message received on UART| | | | |
| | | |val|U32||The bad divider value|
|RD_LedBlinkState|7 (0x7)|Blink state set to new value| | | | |
| | | |val|LedStatePrmEv||The blink state|
