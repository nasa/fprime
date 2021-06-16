module Svc {

  active component CmdSequencer {

    @ FPP from XML: original path was Svc/CmdSequencer/Commands.xml
    include "Commands.fppi"

    @ FPP from XML: original path was Svc/CmdSequencer/Telemetry.xml
    include "Telemetry.fppi"

    @ FPP from XML: original path was Svc/CmdSequencer/Events.xml
    include "Events.fppi"

    command reg port cmdRegOut

    text event port LogText

    async input port pingIn: [1] Svc.Ping

    telemetry port tlmOut

    command resp port cmdResponseOut

    time get port timeCaller

    output port comCmdOut: [1] Fw.Com

    output port pingOut: [1] Svc.Ping

    async input port cmdResponseIn: [1] Fw.CmdResponse

    command recv port cmdIn

    async input port schedIn: [1] Svc.Sched

    async input port seqRunIn: [1] Svc.CmdSeqIn

    event port logOut

    output port seqDone: [1] Fw.CmdResponse

  }

}
