<title>EPSReceiver Component Dictionary</title>
# EPSReceiver Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|ER_SET_FACTOR1|0 (0x0)|Set Operation multiplication factor1| | |   
| | | |val|F32|The first factor|                    
|ER_CLEAR_EVENT_THROTTLE|1 (0x1)|Clear the event throttle| | |   

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|ER_OPERATION|0 (0x0)|Ref::EPSPowerOp|The operation|
|ER_FACTOR1S|1 (0x1)|U32|The number of ER_SET_FACTOR1 commands|
|ER_FACTOR1|2 (0x2)|F32|Factor 1 value|
|ER_FACTOR2|3 (0x3)|F32|Factor 2 value|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|ER_SET_FACTOR1|0 (0x0)|Operation factor 1| | | | |
| | | |val|F32||The factor value|    
|ER_UPDATED_FACTOR2|1 (0x1)|Updated factor 2| | | | |
| | | |val|F32||The factor value|    
|ER_OPERATION_PERFORMED|2 (0x2)|Math Operation performed| | | | |
| | | |val|Ref::EPSPowerOp||The operation|    
|ER_THROTTLE_CLEARED|3 (0x3)|Event throttle cleared| | | | |
