<title>MathSender Component Dictionary</title>
# MathSender Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|MS_DO_MATH|0 (0x0)|Do a math operation| | |   
| | | |val1|F32|The first value|                    
| | | |val2|F32|The second value|                    
| | | |operation|MathOp|The operation to perform|                    

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|MS_VAL1|0 (0x0)|F32|The first value|
|MS_VAL2|1 (0x1)|F32|The second value|
|MS_OP|2 (0x2)|MathOpTlm|The operation|
|MS_RES|3 (0x3)|F32|The result|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|MS_COMMAND_RECV|0 (0x0)|Math command received| | | | |
| | | |val1|F32||The val1 argument|    
| | | |val2|F32||The val1 argument|    
| | | |op|MathOpEv||The requested operation|    
|MS_RESULT|1 (0x1)|Received math result| | | | |
| | | |result|F32||The math result|    
