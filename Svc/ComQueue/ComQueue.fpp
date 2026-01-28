module Svc {
    @ An enumeration of queue data types
    enum QueueType : U8 { COM_QUEUE, BUFFER_QUEUE }

    @ Array of queue depths for Fw::Com types
    array ComQueueDepth = [ComQueueComPorts] U32

    @ Array of queue depths for Fw::Buffer types
    array BuffQueueDepth = [ComQueueBufferPorts] U32

    @ Component used to queue buffer types
    active component ComQueue {

      # ----------------------------------------------------------------------
      # General ports
      # ----------------------------------------------------------------------

      @ Port for emitting data ready to be sent
      output port dataOut: Svc.ComDataWithContext

      @ Port for receiving the status signal
      async input port comStatusIn: Fw.SuccessCondition

      @ Port array for receiving Fw::ComBuffers
      async input port comPacketQueueIn: [ComQueueComPorts] Fw.Com drop

      @ Port array for receiving Fw::Buffers
      async input port bufferQueueIn: [ComQueueBufferPorts] Fw.BufferSend hook

      @ Port array for returning ownership of Fw::Buffer to its original sender
      output port bufferReturnOut: [ComQueueBufferPorts] Fw.BufferSend

      @ Port for receiving Fw::Buffer whose ownership needs to be handed back
      sync input port dataReturnIn: Svc.ComDataWithContext

      @ Port for scheduling telemetry output
      async input port run: Svc.Sched drop

      @ Flush a specific queue. This will discard all queued data in the specified queue removing it from eventual
      @ downlink. Buffers requiring ownership return will be returned via the bufferReturnOut port.
      async command FLUSH_QUEUE(queueType: QueueType @< The Queue data type
                                indexType: FwIndexType @< The index of the queue (within the supplied type) to flush
                               )
      @ Flush all queues. This will discard all queued data removing it from eventual downlink. Buffers requiring
      @ ownership return will be returned via the bufferReturnOut port.
      async command FLUSH_ALL_QUEUES()
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

      @ Command receive port
      command recv port CmdDisp

      @ Command registration port
      command reg port CmdReg

      @ Command response port
      command resp port CmdStatus

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
