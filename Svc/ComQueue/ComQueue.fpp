module Svc {
    @ An enumeration of queue data types
    enum QueueType { COM_QUEUE, BUFFER_QUEUE }

    @ Array of queue depths for Fw::Com types
    array ComQueueDepth = [ComQueueComPorts] U32
    
    @ Array of queue depths for Fw::Buffer types
    array BuffQueueDepth = [ComQueueBufferPorts] U32


    @ Component used to queue buffer types
    active component ComQueue {

      # ----------------------------------------------------------------------
      # General ports
      # ----------------------------------------------------------------------

      @ Fw::ComBuffer output port
      output port comQueueSend: Fw.Com

      @ Fw::Buffer output port
      output port buffQueueSend: Fw.BufferSend

      @ Port for receiving the status signal
      async input port comStatusIn: Fw.SuccessCondition

      @ Port array for receiving Fw::ComBuffers
      async input port comQueueIn: [ComQueueComPorts] Fw.Com drop

      @ Port array for receiving Fw::Buffers
      async input port buffQueueIn: [ComQueueBufferPorts] Fw.BufferSend drop

      @ Port for scheduling telemetry output
      async input port run: Svc.Sched drop

      # ----------------------------------------------------------------------
      # Special ports
      # ----------------------------------------------------------------------

      @ Port for emitting events
      event port Log

      @ Port for emitting text events
      text event port LogText

      @ Port for getting the time
      time get port Time

      @ Port for emitting telemetry
      telemetry port Tlm

      # ----------------------------------------------------------------------
      # Events
      # ----------------------------------------------------------------------

      @ Queue overflow event
      event QueueOverflow(
            queueType: QueueType @< The Queue data type
            index: U32 @< index of overflowed queue
       ) \
        severity warning high \
        format "The {} queue at index {} overflowed"

      # ----------------------------------------------------------------------
      # Telemetry
      # ----------------------------------------------------------------------

      @ Depth of queues of Fw::ComBuffer type
      telemetry comQueueDepth: ComQueueDepth id 0

      @ Depth of queues of Fw::Buffer type
      telemetry buffQueueDepth: BuffQueueDepth id 1
    }
}
