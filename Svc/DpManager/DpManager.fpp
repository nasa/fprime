module Svc {

  @ A component for managing data products
  active component DpManager {

    # ----------------------------------------------------------------------
    # Scheduling ports
    # ----------------------------------------------------------------------

    @ Schedule in port
    async input port schedIn: Svc.Sched drop

    # ----------------------------------------------------------------------
    # Ports for handling buffer requests
    # ----------------------------------------------------------------------

    @ Ports for responding to a data product get from a client component
    sync input port productGetIn: [DpManagerNumPorts] Fw.DpGet

    @ Ports for receiving data product buffer requests from a client component
    async input port productRequestIn: [DpManagerNumPorts] Fw.DpRequest hook

    @ Ports for sending requested data product buffers to a client component
    output port productResponseOut: [DpManagerNumPorts] Fw.DpResponse

    @ Ports for getting buffers from a Buffer Manager
    output port bufferGetOut: [DpManagerNumPorts] Fw.BufferGet

    # ----------------------------------------------------------------------
    # Ports for forwarding filled data products
    # ----------------------------------------------------------------------

    @ Ports for receiving filled data product buffers from a client component
    async input port productSendIn: [DpManagerNumPorts] Fw.DpSend hook

    @ Ports for sending filled data product buffers to a downstream component
    output port productSendOut: [DpManagerNumPorts] Fw.BufferSend

    @ Port for returning a dropped buffer to its pool (overflow hook, avoids leaks)
    output port bufferReturnOut: Fw.BufferSend

    # ----------------------------------------------------------------------
    # F' special ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port cmdIn

    @ Command registration port
    command reg port cmdRegIn

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port eventOut

    @ Telemetry port
    telemetry port tlmOut

    @ Text event port
    text event port textEventOut

    @ Time get port
    time get port timeGetOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Clear event throttling
    async command CLEAR_EVENT_THROTTLE opcode 0x00

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Buffer allocation failed
    event BufferAllocationFailed(
                                  $id: FwDpIdType @< The container ID
                                ) \
      severity warning high \
      format "Buffer allocation failed for container id {}" \
      throttle 10

    @ A filled data product was dropped because the input queue was full (overflow hook)
    event BufferDropped(
                         $id: FwDpIdType @< The container ID
                         $size: FwSizeType @< The dropped buffer size
                       ) \
      severity warning high \
      format "Dropped filled data product for container id {} of size {} on queue overflow" \
      throttle 10

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ The number of successful buffer allocations
    telemetry NumSuccessfulAllocations: U32 update on change

    @ The number of failed buffer allocations
    telemetry NumFailedAllocations: U32 update on change

    @ Number of data products handled
    telemetry NumDataProducts: U32 update on change

    @ Number of bytes handled
    telemetry NumBytes: U64 update on change

  }

}
