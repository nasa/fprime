@ The Buffer Accumulator instance accepted and enqueued a buffer. To avoid uncontrolled sending of events, this event occurs only when the previous buffer received caused a QueueFull error.
event BA_BufferAccepted \
  severity activity high \
  id 0x00 \
  format "Buffer accepted"

@ The Buffer Accumulator instance received a buffer when its queue was full. To avoid uncontrolled sending of events, this event occurs only when the previous buffer received did not cause a QueueFull error.
event BA_QueueFull \
  severity warning high \
  id 0x01 \
  format "Queue full"
