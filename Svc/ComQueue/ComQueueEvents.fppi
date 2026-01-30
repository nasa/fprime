@ Queue overflow event
event QueueOverflow(
    queueType: QueueType @< The Queue data type
    index: U32 @< index of overflowed queue
) \
severity warning high \
format "The {} queue at index {} overflowed"

@ Queue priority changed event
event QueuePriorityChanged(
    queueIndex: U32 @< Index of the queue
    newPriority: U32 @< New priority value
) \
severity activity high \
format "Queue {} priority changed to {}"

@ Invalid queue index error
event InvalidQueueIndex(
    queueIndex: U32 @< Invalid queue index
    maxIndex: U32 @< Maximum valid index
) \
severity warning high \
format "Invalid queue index {} (max: {})"