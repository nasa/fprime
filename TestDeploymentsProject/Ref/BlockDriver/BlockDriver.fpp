module Ref {

  @ An example block driver component with data buffers and interrupts
  active component BlockDriver {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    import Drv.Tick

    @ The rate group scheduler input
    sync input port Sched: Svc.Sched

    @ Internal port that performs the Sched work on the component thread
    internal port SchedInternal hook

    @ The input data buffer port
    async input port BufferIn: Drv.DataBuffer

    @ The output data buffer port
    output port BufferOut: Drv.DataBuffer

    @ Input ping port
    async input port PingIn: Svc.Ping drop

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
    # Telemetry
    # ----------------------------------------------------------------------

    include "Tlm.fppi"

  }

}
