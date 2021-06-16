module Svc {

  @ A component for running command sequences
  active component CmdSequencer {

    # ----------------------------------------------------------------------
    # Special ports 
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port cmdIn

    @ Command resgistration port
    command reg port cmdRegOut

    @ Command response port
    command resp port cmdResponseOut

    @ Event port
    event port logOut

    @ Telemetry port
    telemetry port tlmOut

    @ Text event port
    text event port LogText

    @ Time get port
    time get port timeCaller

    # ----------------------------------------------------------------------
    # General ports 
    # ----------------------------------------------------------------------

    @ Command response in port
    async input port cmdResponseIn: Fw.CmdResponse

    @ Ping in port
    async input port pingIn: Svc.Ping

    @ Ping out port
    output port pingOut: Svc.Ping

    @ Port for indicating sequence done
    output port seqDone: Fw.CmdResponse

    @ Port for requests to run sequences
    async input port seqRunIn: Svc.CmdSeqIn

    @ Port for sending sequence commands
    output port comCmdOut: Fw.Com

    @ Schedule in port
    async input port schedIn: Svc.Sched

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
