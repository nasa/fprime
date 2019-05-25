<title>RecvBuff Component Dictionary</title>
# RecvBuff Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|PktState|0 (0x0)|Ref::PacketStat|Packet Statistics|
|Sensor1|1 (0x1)|F32|Value of Sensor1|
|Sensor2|2 (0x2)|F32|Value of Sensor3|
|Parameter1|3 (0x3)|U32|Readback of Parameter1|
|Parameter2|4 (0x4)|I16|Readback of Parameter2|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|FirstPacketReceived|0 (0x0)|First packet received| | | | |
| | | |id|U32||The ID argument|
|PacketChecksumError|1 (0x1)|Packet checksum error| | | | |
| | | |id|U32||The ID argument|
|BuffRecvParameterUpdated|2 (0x2)|Report parameter update| | | | |
| | | |id|U32||The ID argument|
