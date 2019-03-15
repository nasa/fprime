<title>Example Component Dictionary</title>
# Example Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|TEST_CMD_1|256 (0x100)|A test command| | |
| | | |arg1|I32|The I32 command argument|
| | | |arg2|CmdEnum|The ENUM argument|
| | | |arg3|Fw::CmdStringArg|The string argument|
|TEST_CMD_2|257 (0x101)|A test command| | |
| | | |arg1|I32|The I32 command argument|
| | | |arg2|F32|A float argument|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|somechan|100 (0x64)|U32|A test channel|
|anotherchan|101 (0x65)|U32|A test channel|
|stringchan|102 (0x66)|U32|A test channel|
|enumchan|103 (0x67)|SomeTlmEnum|A test enum channel|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|SomeEvent|100 (0x64)|A test event| | | | |
| | | |arg1|I32||The I32 event argument|
| | | |arg2|F32||The F32 event argument|
| | | |arg3|U32||The U32 event argument|
| | | |arg4|Fw::LogStringArg&|12|The string event argument|
| | | |arg5|SomeEventEnum||The enum event argument|
