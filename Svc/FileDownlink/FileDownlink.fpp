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

    async input port Run: [1] Svc.Sched

    guarded input port SendFile: [1] Svc.SendFileRequest

    output port FileComplete: [FileDownCompletePorts] Svc.SendFileComplete

    async input port bufferReturn: [1] Fw.BufferSend

    output port bufferSendOut: [1] Fw.BufferSend

    async input port pingIn: [1] Svc.Ping

    output port pingOut: [1] Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port CmdDisp

    @ Command registration port
    command reg port CmdReg

    @ Command response port
    command resp port CmdStatus

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    @ Telemetry port
    telemetry port Tlm

  }

}
