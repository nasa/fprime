@ Flush a specific queue. This will discard all queued data in the specified queue removing it from eventual
@ downlink. Buffers requiring ownership return will be returned via the bufferReturnOut port.
async command FLUSH_QUEUE(queueType: QueueType @< The Queue data type
                        indexType: FwIndexType @< The index of the queue (within the supplied type) to flush
                        )

@ Flush all queues. This will discard all queued data removing it from eventual downlink. Buffers requiring
@ ownership return will be returned via the bufferReturnOut port.
async command FLUSH_ALL_QUEUES()

@ Set the priority of a specific queue at runtime
async command SET_QUEUE_PRIORITY(
                                queueType: QueueType @< The Queue data type
                                indexType: FwIndexType @< The index of the queue (within the supplied type) to modify
                                newPriority: FwIndexType @< New priority value for the queue
                                )

