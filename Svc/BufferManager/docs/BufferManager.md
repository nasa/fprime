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
|BufferManager_AllocationQueueEmpty|0 (0x0)|The Buffer Manager received a request to deallocate a buffer when the allocation queue was empty| | | | |
|BufferManager_AllocationQueueFull|1 (0x1)|The Buffer Manager received an allocation request that, if granted, would cause the allocation queue to overflow| | | | |
|BufferManager_IDMismatch|2 (0x2)|The Buffer Manager received a deallocation request whose ID did not match the ID at the end of the allocation queue| | | | |
| | | |expected|U32||The expected ID value|    
| | | |saw|U32||The ID value seen|    
|BufferManager_StoreSizeExceeded|3 (0x3)|The Buffer Manager received an allocation request that, if granted, would exceed the store size| | | | |
