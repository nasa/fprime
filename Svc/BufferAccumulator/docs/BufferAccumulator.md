<title>BufferAccumulator Component Dictionary</title>
# BufferAccumulator Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|BA_SetMode|0 (0x0)|Set the mode| | |   
| | | |mode|OpState||                    
|BA_DrainBuffers|1 (0x1)|Drain the commanded number of buffers| | |   
| | | |numToDrain|U32||                    
| | | |blockMode|BlockMode||                    

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|BA_NumQueuedBuffers|0 (0x0)|U32|The number of buffers queued|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|BA_BufferAccepted|0 (0x0)|The Buffer Accumulator instance accepted and enqueued a buffer. To avoid uncontrolled sending of events, this event occurs only when the previous buffer received caused a QueueFull error.| | | | |
|BA_QueueFull|1 (0x1)|The Buffer Accumulator instance received a buffer when its queue was full. To avoid uncontrolled sending of events, this event occurs only when the previous buffer received did not cause a QueueFull error.| | | | |
|BA_StillDraining|2 (0x2)|Got DrainBuffers command while executing DrainBuffers command| | | | |
| | | |numDrained|U32|||    
| | | |numToDrain|U32|||    
|BA_AlreadyDraining|3 (0x3)|Got DrainBuffers command while in DRAIN mode| | | | |
|BA_DrainStalled|4 (0x4)|Ran out of buffers while executing DrainBuffers command| | | | |
| | | |numDrained|U32|||    
| | | |numToDrain|U32|||    
|BA_PartialDrainDone|5 (0x5)|Finished DrainBuffers command| | | | |
| | | |numDrained|U32|||    
|BA_NonBlockDrain|6 (0x6)|Not enough buffers to complete requested drain, and NOBLOCK was set; will only drain what we have| | | | |
| | | |numWillDrain|U32|||    
| | | |numReqDrain|U32|||    
