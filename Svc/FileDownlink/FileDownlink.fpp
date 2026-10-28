module Svc {

  @ A component for downlinking files
  active component FileDownlink {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Count scheduler ticks and coalesce state-machine work.
    sync input port Run: Svc.Sched

    @ Process state-machine work in the component task; retry on a later tick if full.
    internal port runWork hook

    @ Mutexed Sendfile input port
    guarded input port SendFile: Svc.SendFileRequest

    @ File complete output port
    output port FileComplete: [FileDownCompletePorts] Svc.SendFileComplete

    @ Buffer return input port
    async input port bufferReturn: Fw.BufferSend

    @ Buffer send output port
    output port bufferSendOut: Fw.BufferSend

    @ Ping input port
    async input port pingIn: Svc.Ping drop

    @ Ping output port
    output port pingOut: Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Time get port
    time get port timeCaller

    @ Command registration port
    command reg port cmdRegOut

    @ Command receive port
    command recv port cmdIn

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port eventOut

    @ Text event port
    text event port textEventOut

    @ Telemetry port
    telemetry port tlmOut

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    include "Commands.fppi"

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    include "Telemetry.fppi"

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "Events.fppi"

  }

}
