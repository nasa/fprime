<title>PingReceiver Component Dictionary</title>
# PingReceiver Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|PR_StopPings|0 (0x0)|Command to disable ping response| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|PR_NumPings|0 (0x0)|U32|Number of pings received|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|PR_PingsDisabled|0 (0x0)|Disabled ping responses| | | | |
|PR_PingReceived|1 (0x1)|Got ping| | | | |
| | | |code|U32||Ping code|
