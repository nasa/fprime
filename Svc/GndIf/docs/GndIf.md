<title>GndIf Component Dictionary</title>
# GndIf Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|GNDIF_ENABLE_INTERFACE|0 (0x0)|Enable the interface for ground communications| | |


## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|NoConnectionToServer|0 (0x0)|The thread will try to re-connect until a connection is made| | | | |
| | | |PortNumber|U32||The port number running the socket server|
|ConnectedToServer|1 (0x1)|The socket thread will continuously read until disconnected or terminated| | | | |
| | | |PortNumber|U32||The port number running the socket server|
|LostConnectionToServer|2 (0x2)|The socket thread will continuously try to reconnect to socket server if connection dropped by server| | | | |
| | | |PortNumber|U32||The port number running the socket server|
|GNDIF_ReceiveError|3 (0x3)|Error in GndIf receiving packet| | | | |
| | | |error|GNDIF_RECEIVE_ERROR||The error code|
| | | |PortNumber|U32||The port number running the socket server|
