<title>BufferManager Component Dictionary</title>
# BufferManager Component Dictionary


## Telemetry Channel List

|Channel Name|ID|Type|Description|
|---|---|---|---|
|BufferManager_NumAllocatedBuffers|0 (0x0)|U32|The number of buffers currently allocated|
|BufferManager_AllocatedSize|1 (0x1)|U32|The total size of all allocated buffers|

## Event List

|Event Name|ID|Description|Arg Name|Arg Type|Arg Size|Description
|---|---|---|---|---|---|---|
|ClearedErrorState|0 (0x0)|The Buffer Manager has cleared its error state| | | | |
|StoreSizeExceeded|1 (0x1)|The Buffer Manager received an allocation request that, if granted, would cause the storage buffer to overflow| | | | |
|TooManyBuffers|2 (0x2)|The Buffer Manager received an allocation request that, if granted, would result in too many buffers| | | | |
