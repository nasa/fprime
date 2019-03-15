<title>SendBuff Component Dictionary</title>
# SendBuff Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|SB_START_PKTS|0 (0x0)|Command to start sending packets| | |
|SB_INJECT_PKT_ERROR|1 (0x1)|Send a bad packet| | |
|SB_GEN_FATAL|2 (0x2)|Generate a FATAL EVR| | |
| | | |arg1|U32|First FATAL Argument|
| | | |arg2|U32|Second FATAL Argument|
| | | |arg3|U32|Third FATAL Argument|
|SB_GEN_ASSERT|3 (0x3)|Generate an ASSERT| | |
| | | |arg1|U32|First ASSERT Argument|
| | | |arg2|U32|Second ASSERT Argument|
| | | |arg3|U32|Third ASSERT Argument|
| | | |arg4|U32|Fourth ASSERT Argument|
| | | |arg5|U32|Fifth ASSERT Argument|
| | | |arg6|U32|Sixth ASSERT Argument|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|PacketsSent|0 (0x0)|U64|Number of packets sent|
|NumErrorsInjected|1 (0x1)|U32|Number of errors injected|
|Parameter3|2 (0x2)|U8|Readback of Parameter3|
|Parameter4|3 (0x3)|F32|Readback of Parameter4|
|SendState|4 (0x4)|ActiveState|Readback of Parameter4|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|FirstPacketSent|0 (0x0)|First packet send| | | | |
| | | |id|U32||The ID argument|
|PacketErrorInserted|1 (0x1)|Packet checksum error| | | | |
| | | |id|U32||The ID argument|
|BuffSendParameterUpdated|2 (0x2)|Report parameter update| | | | |
| | | |id|U32||The ID argument|
|SendBuffFatal|3 (0x3)|A test FATAL| | | | |
| | | |arg1|U32||First FATAL argument|
| | | |arg2|U32||Second FATAL argument|
| | | |arg3|U32||Second FATAL argument|
