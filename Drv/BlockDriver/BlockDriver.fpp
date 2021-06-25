module Drv {

  @ An example block driver component with data buffers and interrupts
  active component BlockDriver {

    @ FPP from XML: original path was Drv/BlockDriver/Tlm.xml
    include "Tlm.fppi"

    @ The rate group scheduler input
    async input port Sched: [1] Svc.Sched

    @ The input data buffer port
    async input port BufferIn: [1] Drv.DataBuffer

    @ The cycle outputs. Meant to be connected to rate group driver
    output port CycleOut: [1] Svc.Cycle

    @ The output data buffer port
    output port BufferOut: [1] Drv.DataBuffer

    @ Input ping port
    async input port PingIn: [1] Svc.Ping

    @ Output ping port
    output port PingOut: [1] Svc.Ping

    @ Internal interrupt reporting interface
    internal port InterruptReport(
                                   interrupt: U32 @< The interrupt register value
                                 ) \
      priority 1

    @ Time get port
    time get port Time

    @ Telemetry port
    telemetry port Tlm

  }

}
