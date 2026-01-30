@ Set the priority of a specific queue at runtime
async command SET_QUEUE_PRIORITY(
                                queueIndex: U32 @< Index of the queue to modify
                                newPriority: U32 @< New priority value for the queue
                                ) \
opcode 0x00


@ Flush a specific queue. This will discard all queued data in the specified queue removing it from eventual
@ downlink. Buffers requiring ownership return will be returned via the bufferReturnOut port.
async command FLUSH_QUEUE(queueType: QueueType @< The Queue data type
                        indexType: FwIndexType @< The index of the queue (within the supplied type) to flush
                        ) \
opcode 0x01

@ Flush all queues. This will discard all queued data removing it from eventual downlink. Buffers requiring
@ ownership return will be returned via the bufferReturnOut port.
async command FLUSH_ALL_QUEUES() opcode 0x02