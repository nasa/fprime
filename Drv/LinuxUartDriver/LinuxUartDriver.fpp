module Drv {

  passive component LinuxUartDriver {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    import ByteStreamDriver

    @ Allocation port used for allocating memory in the receive task
    output port allocate: Fw.BufferGet

    @ Deallocation of allocated buffers
    output port deallocate: Fw.BufferSend

    @ The rate group input for sending telemetry
    sync input port run: Svc.Sched

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
