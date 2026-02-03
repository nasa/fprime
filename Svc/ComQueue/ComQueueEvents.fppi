@ Queue overflow event
event QueueOverflow(
    queueType: QueueType @< The Queue data type
    index: FwIndexType @< index of overflowed queue
) \
severity warning high \
format "The {} queue at index {} overflowed"

@ Queue priority changed event
event QueuePriorityChanged(
    queueType: QueueType @< The Queue data type
    indexType: FwIndexType @< The index of the queue (within the supplied type) that was modified
    newPriority: FwIndexType @< New priority value
) \
severity activity high \
format "{} {} priority changed to {}"