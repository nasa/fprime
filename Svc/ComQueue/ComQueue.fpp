module Svc {

    @ A component for managing Comm buffers on a queue
    active component ComQueue {

      # ----------------------------------------------------------------------
      # Types
      # ----------------------------------------------------------------------

      @ The state enumeration of the queue
      enum ComStatus {
         SUCCESS = 0 @< state to empty the head of comm buffer
         FAIL = 1 @< state to maintain status of com buffer
      }

      # ----------------------------------------------------------------------
      # General ports
      # ----------------------------------------------------------------------

      @ ComQueue output port, output the head of the Com buffer
      output port comQueueSend: [1] Fw.Com

      @ Port for receiving the status signal
      async input port comStatusIn: Fw.Com


      # ----------------------------------------------------------------------
      # Internal ports
      # ----------------------------------------------------------------------

      @ Port that puts the Com buffer to component thread
      internal port ComQueue(
                            $id: FwEventIdType @< Log ID
                            timeTag: Fw.Time @< Time Tag
                            $severity: Fw.LogSeverity @< The severity argument
                            args: Fw.LogBuffer @< Buffer containing serialized log entry
                            ) \
        priority 1 \
        drop
      # ----------------------------------------------------------------------
      # Special ports
      # ----------------------------------------------------------------------

      @ A port for receiving commands
      command recv port cmdIn

      @ A port for sending command registration requests
      command reg port cmdRegOut

      @ A port for sending command responses
      command resp port cmdResponseOut

      @ Port for emitting events
      event port Log

      @ Port for emitting text events
      text event port LogText

      @ Parameter get Queue Depth
      param get port prmGetQueueDepth

      @ Parameter set Queue Depth
      param set port prmSetQueueDepth

      @ Port for getting the time
      time get port Time

      @ Telemetry port
      telemetry port Tlm

      # ----------------------------------------------------------------------
      # Commands
      # ----------------------------------------------------------------------

      @ Command to release the head of Com buffer
      async command Empty

      # ----------------------------------------------------------------------
      # Parameters
      # ----------------------------------------------------------------------

      @ Size of queue
      param QueueDepth: U32

      # ----------------------------------------------------------------------
      # Events
      # ----------------------------------------------------------------------

      @ Queue is full event
      event QueueFull severity warning high \
        format "The Queue is full" \
        throttle 10

      # ----------------------------------------------------------------------
      # Telemetry
      # ----------------------------------------------------------------------

      @ Number of items in queue
      telemetry ItemsInQueue: U32 id 0 update on change
    }
}