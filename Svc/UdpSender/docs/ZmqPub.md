<title>ZmqPub Component Dictionary</title>
# ZmqPub Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|ZP_PacketsSent|0 (0x0)|U32|Number of packets sent|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|ZP_PublishConnectionOpened|0 (0x0)|Connection opened| | | | |
|ZP_ContextError|1 (0x1)|Zmq publisher context open error| | | | |
| | | |error|Fw::LogStringArg&|80||    
|ZP_SocketError|2 (0x2)|Zmq publisher socket error| | | | |
| | | |error|Fw::LogStringArg&|80||    
|ZP_BindError|3 (0x3)|Zmq publisher bind error| | | | |
| | | |error|Fw::LogStringArg&|80||    
|ZP_SendError|4 (0x4)|Zmq publisher send error| | | | |
| | | |error|Fw::LogStringArg&|80||    
