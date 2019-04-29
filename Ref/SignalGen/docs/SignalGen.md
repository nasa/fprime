<title>SignalGen Component Dictionary</title>
# SignalGen Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|SignalGen_Settings|0 (0x0)|Signal Generator Settings| | |
| | | |Frequency|U32||
| | | |Amplitude|U32||
| | | |Phase|U32||
|SignalGen_Toggle|1 (0x1)|Toggle Signal Generator On/Off.| | |
|SignalGen_Skip|2 (0x2)|Skip next sample| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|SignalGen_Output|0 (0x0)|F32|SignalGen Output|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|SignalGen_SettingsChanged|0 (0x0)|Signal Generator Settings Changed| | | | |
| | | |Frequency|U32|||
| | | |Amplitude|U32|||
| | | |Phase|U32|||
