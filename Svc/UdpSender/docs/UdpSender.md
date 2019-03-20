<title>UdpSender Component Dictionary</title>
# UdpSender Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|US_PacketsSent|0 (0x0)|U32|Number of packets sent|
|US_BytesSent|1 (0x1)|U32|Number of bytes sent|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|US_PortOpened|0 (0x0)|Connection opened| | | | |
| | | |server|Fw::LogStringArg&|80||
| | | |port|U32|||
|US_SocketError|1 (0x1)|UDP port socket error| | | | |
| | | |error|Fw::LogStringArg&|80||
|US_SendError|2 (0x2)|UDP send error| | | | |
| | | |error|Fw::LogStringArg&|80||
