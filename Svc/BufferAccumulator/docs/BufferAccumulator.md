<title>BufferAccumulator Component Dictionary</title>
# BufferAccumulator Component Dictionary


## Command List

|Mnemonic|ID|Description|Arg Name|Arg Type|Comment
|---|---|---|---|---|---|
|BA_SetMode|0 (0x0)|Set the mode| | |
| | | |mode|OpState||

## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|BufferAccumulator_NumQueuedBuffers|0 (0x0)|U32|The number of buffers queued|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|BA_BufferAccepted|0 (0x0)|The Buffer Accumulator instance accepted and enqueued a buffer. To avoid uncontrolled sending of events, this event occurs only when the previous buffer received caused a QueueFull error.| | | | |
|BA_QueueFull|1 (0x1)|The Buffer Accumulator instance received a buffer when its queue was full. To avoid uncontrolled sending of events, this event occurs only when the previous buffer received did not cause a QueueFull error.| | | | |
