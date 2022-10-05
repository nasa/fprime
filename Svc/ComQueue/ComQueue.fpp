module Svc {
    @ An enumeration of queue types
    enum QueueType { comQueue, buffQueue }

    array ComQueueDepth = [ComQueueComSize] U32
    array BuffQueueDepth = [ComQueueBuffSize] U32


    @ A component for managing Comm buffers on a queue
    active component ComQueue {

      # ----------------------------------------------------------------------
      # General ports
      # ----------------------------------------------------------------------

      @ ComQueue com output port, output the head of the Com buffer
      output port comQueueSend: Fw.Com

      @ ComQueue buffer output port, output the head of Fw.buffer
      output port buffQueueSend: Fw.BufferSend

      @ Port for receiving the status signal
      async input port comStatusIn: ComStatus

      @ Port for receiving Com buffer
      async input port comQueueIn: [ComQueueComSize] Fw.Com

      @ Port for receiving Fw.buffer
      async input port buffQueueIn: [ComQueueBuffSize] Fw.BufferSend

      @ Port for emitting telemetry
      async input port run: Svc.Sched

      @ Port for receiving Fw.Buffers back for retry
      sync input port retryReturn: Fw.BufferSend

      @ Port for deallocating Fw.Buffers back for retry
      output port retryDeallocate: Fw.BufferSend

      # ----------------------------------------------------------------------
      # Special ports
      # ----------------------------------------------------------------------

      @ Port for emitting events
      event port Log

      @ Port for emitting text events
      text event port LogText

      @ Port for getting the time
      time get port Time

      @ Telemetry port
      telemetry port Tlm

      # ----------------------------------------------------------------------
      # Events
      # ----------------------------------------------------------------------

      @ Queue is full event
      event QueueFull(
            queueType: QueueType @< The Queue type
            index: U32 @< index value
       ) \
        severity warning high \
        format "The {} queue at index {} is full" \

      # ----------------------------------------------------------------------
      # Telemetry
      # ----------------------------------------------------------------------

      @ How close the com queue is to being full
      telemetry comQueueDepth: ComQueueDepth id 0

      @ How close the buf queue is to being full
      telemetry buffQueueDepth: BuffQueueDepth id 1
    }
}
