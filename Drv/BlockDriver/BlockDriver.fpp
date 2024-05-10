module Drv {

  @ An example block driver component with data buffers and interrupts
  active component BlockDriver {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    include "../Interfaces/TickInterface.fppi"

    @ The rate group scheduler input
    async input port Sched: Svc.Sched

    @ The input data buffer port
    async input port BufferIn: Drv.DataBuffer

    @ The output data buffer port
    output port BufferOut: Drv.DataBuffer

    @ Input ping port
    async input port PingIn: Svc.Ping

    @ Output ping port
    output port PingOut: Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Time get port
    time get port Time

    @ Telemetry port
    telemetry port Tlm

    # ----------------------------------------------------------------------
    # Internal ports
    # ----------------------------------------------------------------------

    @ Internal interrupt reporting interface
    internal port InterruptReport(
                                   interrupt: U32 @< The interrupt register value
                                 ) \
      priority 1

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    include "Tlm.fppi"

  }

}
