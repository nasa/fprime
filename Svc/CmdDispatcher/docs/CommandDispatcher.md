<title>CommandDispatcher Component Dictionary</title>
# CommandDispatcher Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|CMD_NO_OP|0 (0x0)|No-op command| | |
|CMD_NO_OP_STRING|1 (0x1)|No-op string command| | |
| | | |arg1|Fw::CmdStringArg|The String command argument|
|CMD_TEST_CMD_1|2 (0x2)|No-op command| | |
| | | |arg1|I32|The I32 command argument|
| | | |arg2|F32|The F32 command argument|
| | | |arg3|U8|The U8 command argument|
|CMD_CLEAR_TRACKING|3 (0x3)|Clear command tracking info to recover from components not returning status| | |

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|CommandsDispatched|0 (0x0)|U32|Number of commands dispatched|
|CommandErrors|1 (0x1)|U32|Number of command errors|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|OpCodeRegistered|0 (0x0)|Op code registered event| | | | |
| | | |Opcode|U32||The opcode to register|
| | | |port|I32||The registration port|
| | | |slot|I32||The dispatch slot it was placed in|
|OpCodeDispatched|1 (0x1)|Op code dispatched event| | | | |
| | | |Opcode|U32||The opcode dispatched|
| | | |port|I32||The port dispatched to|
|OpCodeCompleted|2 (0x2)|Op code completed event| | | | |
| | | |Opcode|U32||The I32 command argument|
|OpCodeError|3 (0x3)|Op code completed with error event| | | | |
| | | |Opcode|U32||The opcode with the error|
| | | |error|ErrorResponse||The error value|
|MalformedCommand|4 (0x4)|Received a malformed command packet| | | | |
| | | |Status|CmdSerError||The deserialization error|
|InvalidCommand|5 (0x5)|Received an invalid opcode| | | | |
| | | |Opcode|U32||Invalid opcode|
|TooManyCommands|6 (0x6)|Exceeded the number of commands that can be simultaneously executed| | | | |
| | | |Opcode|U32||The opcode that overflowed the list|
|NoOpReceived|7 (0x7)|The command dispatcher has successfully recieved a NO-OP command| | | | |
|NoOpStringReceived|8 (0x8)|The command dispatcher has successfully recieved a NO-OP command from GUI with a string| | | | |
| | | |message|Fw::LogStringArg&|40|The NO-OP string that is generated|
|TestCmd1Args|9 (0x9)|This log event message returns the TEST_CMD_1 arguments.| | | | |
| | | |arg1|I32||Arg1|
| | | |arg2|F32||Arg2|
| | | |arg3|U8||Arg3|
