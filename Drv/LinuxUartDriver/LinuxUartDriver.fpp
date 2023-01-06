module Drv {

  passive component LinuxUartDriver {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Indicates the driver has connected to the UART device
    output port ready: Drv.ByteStreamReady

    @ Produces data received via the UART device on the receive task
    output port $recv: Drv.ByteStreamRecv

    @ Takes data to transmit out the UART device
    guarded input port send: Drv.ByteStreamSend

    @ Allocation port used for allocating memory in the receive task
    output port allocate: Fw.BufferGet

    @ Deallocates buffers passed to the "send" port
    output port deallocate: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    event port Log

    telemetry port Tlm

    text event port LogText

    time get port Time

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    include "Telemetry.fppi"

  }

}
