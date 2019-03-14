<title>BuffGndSockIf Component Dictionary</title>
# BuffGndSockIf Component Dictionary



## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|BuffGndSockIf_NoConnectionToServer|0 (0x0)|The thread will try to re-connect until a connection is made| | | | |
| | | |PortNumber|U32||The port number running the socket server|
|BuffGndSockIf_ConnectedToServer|1 (0x1)|The socket thread will continuously read until disconnected or terminated| | | | |
| | | |PortNumber|U32||The port number running the socket server|
|BuffGndSockIf_LostConnectionToServer|2 (0x2)|The socket thread will continuously try to reconnect to socket server if connection dropped by server| | | | |
| | | |PortNumber|U32||The port number running the socket server|
