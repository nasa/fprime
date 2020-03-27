<title>FileUplink Component Dictionary</title>
# FileUplink Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|FileUplink_FilesReceived|0 (0x0)|U32|The total number of complete files received|
|FileUplink_PacketsReceived|1 (0x1)|U32|The total number of packets received|
|FileUplink_Warnings|2 (0x2)|U32|The total number of warnings issued|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|FileUplink_BadChecksum|0 (0x0)|During receipt of a file, the computed checksum value did not match the stored value| | | | |
| | | |fileName|Fw::LogStringArg&|40|The file name|
| | | |computed|U32||The computed value|
| | | |read|U32||The value read|
|FileUplink_FileOpenError|1 (0x1)|An error occurred opening a file| | | | |
| | | |fileName|Fw::LogStringArg&|40|The name of the file|
|FileUplink_FileReceived|2 (0x2)|The File Uplink component successfully received a file| | | | |
| | | |fileName|Fw::LogStringArg&|40|The name of the file|
|FileUplink_FileWriteError|3 (0x3)|An error occurred writing to a file| | | | |
| | | |fileName|Fw::LogStringArg&|40|The name of the file|
|FileUplink_InvalidReceiveMode|4 (0x4)|The File Uplink component received a packet with a type that was invalid for the current receive mode| | | | |
| | | |packetType|U32||The type of the packet received|
| | | |mode|U32||The receive mode|
|FileUplink_PacketOutOfBounds|5 (0x5)|During receipt of a file, the File Uplink component encountered a packet with offset and size out of bounds for the current file| | | | |
| | | |packetIndex|U32||The sequence index of the packet|
| | | |fileName|Fw::LogStringArg&|40|The name of the file|
|FileUplink_PacketOutOfOrder|6 (0x6)|The File Uplink component encountered an out-of-order packet during file receipt| | | | |
| | | |packetIndex|U32||The sequence index of the out-of-order packet|
| | | |lastPacketIndex|U32||The sequence index of the last packet received before the out-of-order packet|
|FileUplink_UplinkCanceled|7 (0x7)|The File Uplink component received a CANCEL packet| | | | |
