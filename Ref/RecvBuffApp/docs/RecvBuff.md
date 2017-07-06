<title>RecvBuff Component Dictionary</title>
# RecvBuff Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|NumPkts|0 (0x0)|U32|Number of packets received|
|NumErrPkts|1 (0x1)|U32|Number of packets received with errors|
|Sensor1|2 (0x2)|F32|Value of Sensor1|
|Sensor2|3 (0x3)|F32|Value of Sensor3|
|Parameter1|4 (0x4)|U32|Readback of Parameter1|
|Parameter2|5 (0x5)|I16|Readback of Parameter2|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|FirstPacketReceived|0 (0x0)|First packet received| | | | |
| | | |id|U32||The ID argument|    
|PacketChecksumError|1 (0x1)|Packet checksum error| | | | |
| | | |id|U32||The ID argument|    
|BuffRecvParameterUpdated|2 (0x2)|Report parameter update| | | | |
| | | |id|U32||The ID argument|    
