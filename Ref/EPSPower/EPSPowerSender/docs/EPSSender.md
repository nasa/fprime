<title>EPSSender Component Dictionary</title>
# EPSSender Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|ES_DO_MATH|0 (0x0)|Do a math operation| | |   
| | | |val1|F32|The first value|                    
| | | |val2|F32|The second value|                    
| | | |operation|EPSPowerOp|The operation to perform|                    

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|ES_VAL1|0 (0x0)|F32|The first value|
|ES_VAL2|1 (0x1)|F32|The second value|
|ES_OP|2 (0x2)|EPSPowerOpTlm|The operation|
|MS_RES|3 (0x3)|F32|The result|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|ES_COMMAND_RECV|0 (0x0)|Math Command Received| | | | |
| | | |val1|F32||The val1 argument|    
| | | |val2|F32||The val2 argument|    
| | | |op|EPSPowerOpEv||The requested operation|    
|ES_RESULT|1 (0x1)|Received math result| | | | |
| | | |result|F32||The math result|    
