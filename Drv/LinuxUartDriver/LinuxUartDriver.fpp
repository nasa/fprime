module Drv {

  passive component LinuxUartDriver {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    output port ready: Drv.ByteStreamReady

    output port $recv: Drv.ByteStreamRecv

    guarded input port send: Drv.ByteStreamSend

    output port allocate: Fw.BufferGet

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
