# Svc::TlmPacketizer Component Dictionary

## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|TPK_SET_LEVEL|0 (0x0)|Set telemetry send level| | |
| | | |level|U32|The I32 command argument|
|TPK_SEND_PKT|1 (0x1)|Force a packet to be sent| | |
| | | |id|U32|The packet ID|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|TPK_SendLevel|0 (0x0)|U32|Telemetry send level|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|TPK_NoChan|0 (0x0)|Telemetry channel is not part of a telemetry packet.| | | | |
| | | |Id|U32||The telemetry ID|
|TPK_LevelSet|1 (0x1)|Telemetry send level set| | | | |
| | | |id|U32||The level|
|TPK_MaxLevelExceed|2 (0x2)|Telemetry send level set| | | | |
| | | |level|U32||The level|
| | | |max|U32||The max packet level|
|TPK_PacketSent|3 (0x3)|Packet manually sent| | | | |
| | | |id|U32||The packet ID|
|TPK_PacketNotFound|4 (0x4)|Couldn't find the packet to send| | | | |
| | | |id|U32||The packet ID|
