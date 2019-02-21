<title>TestCommand Component Dictionary</title>
# TestCommand Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|TEST_CMD_1|356 (0x164)|A test command| | |
| | | |arg1|I32|The I32 command argument|
| | | |arg2|Ref::Gnc::Quaternion||

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|somechan|200 (0xc8)|Ref::Gnc::Quaternion|A test channel|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|SomeEvent|200 (0xc8)|A test event| | | | |
| | | |arg1|Ref::Gnc::Quaternion||The quaternion command argument|
| | | |arg2|F32||The F32 command argument|
| | | |arg3|U8||The U8 command argument|
