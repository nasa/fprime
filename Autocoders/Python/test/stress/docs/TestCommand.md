<title>TestCommand Component Dictionary</title>
# TestCommand Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|TEST_CMD_1|272 (0x110)|A test command| | |
| | | |arg1|I32|The I32 command argument|
| | | |arg2|SomeEnum|The ENUM argument|
|TEST_CMD_2|273 (0x111)|A test command| | |
| | | |arg1|I32|The I32 command argument|
| | | |arg2|F32|A float argument|

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|somechan|132 (0x84)|U32|A test channel|
|AQuat|133 (0x85)|Ref::Gnc::Quaternion|A test channel|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|SomeEvent|148 (0x94)|A test event| | | | |
| | | |arg1|I32||The I32 command argument|
| | | |arg2|F32||The F32 command argument|
| | | |arg3|U8||The U8 command argument|
|AnotherEvent|149 (0x95)|A test event| | | | |
| | | |arg1|I32||The I32 command argument|
| | | |arg2|F32||The F32 command argument|
| | | |arg3|Ref::Gnc::Quaternion||The U8 command argument|
