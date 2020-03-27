<title>Health Component Dictionary</title>
# Health Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|HLTH_ENABLE|0 (0x0)|A command to enable or disable health checks| | |
| | | |enable|HealthEnabled|whether or not health checks are enabled|
|HLTH_PING_ENABLE|1 (0x1)|Ignore a particular ping entry| | |
| | | |entry|Fw::CmdStringArg|The entry to enable/disable|
| | | |enable|PingEnabled|whether or not a port is pinged|
|HLTH_CHNG_PING|2 (0x2)|Change ping value| | |
| | | |entry|Fw::CmdStringArg|The entry to modify|
| | | |warningValue|U32|Ping warning threshold|
| | | |fatalValue|U32|Ping fatal threshold|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|PingLateWarnings|0 (0x0)|U32|Number of overrun warnings|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|HLTH_PING_WARN|0 (0x0)|Warn that a ping target is longer than the warning value| | | | |
| | | |entry|Fw::LogStringArg&|40|The entry passing the warning level|
|HLTH_PING_LATE|1 (0x1)|Declare FATAL since task is no longer responding| | | | |
| | | |entry|Fw::LogStringArg&|40|The entry passing the warning level|
|HLTH_PING_WRONG_KEY|2 (0x2)|Declare FATAL since task is no longer responding| | | | |
| | | |entry|Fw::LogStringArg&|40|The entry passing the warning level|
| | | |badKey|U32||The incorrect key value|
|HLTH_CHECK_ENABLE|3 (0x3)|Report checking turned on or off| | | | |
| | | |enabled|HealthIsEnabled||If health checking is enabled|
|HLTH_CHECK_PING|4 (0x4)|Report a particular entry on or off| | | | |
| | | |enabled|HealthPingIsEnabled||If health pinging is enabled for a particular entry|
| | | |entry|Fw::LogStringArg&|40|The entry passing the warning level|
|HLTH_CHECK_LOOKUP_ERROR|5 (0x5)|Entry was not found| | | | |
| | | |entry|Fw::LogStringArg&|40|The entry passing the warning level|
|HLTH_PING_UPDATED|6 (0x6)|Report changed ping| | | | |
| | | |entry|Fw::LogStringArg&|40|The entry changed|
| | | |warn|U32||The new warning value|
| | | |fatal|U32||The new FATAL value|
|HLTH_PING_INVALID_VALUES|7 (0x7)|Report changed ping| | | | |
| | | |entry|Fw::LogStringArg&|40|The entry changed|
| | | |warn|U32||The new warning value|
| | | |fatal|U32||The new FATAL value|
