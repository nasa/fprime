module Svc {

  active component FileDownlink {

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

    # ----------------------------------------------------------------------
    # General Ports
    # ----------------------------------------------------------------------

    @ Run input port
    async input port Run: [1] Svc.Sched

    @ Mutexed Sendfile input port
    guarded input port SendFile: [1] Svc.SendFileRequest

    @ File complete output port
    output port FileComplete: [FileDownCompletePorts] Svc.SendFileComplete

    @ Buffer return input port
    async input port bufferReturn: [1] Fw.BufferSend

    @ Buffer send output port
    output port bufferSendOut: [1] Fw.BufferSend

    @ Ping input port
    async input port pingIn: [1] Svc.Ping

    @ Ping output port
    output port pingOut: [1] Svc.Ping

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

  }

}
