module Svc {

  @ A component for storing telemetry
  active component ActiveLogger {

    @ FPP from XML: original path was Svc/ActiveLogger/ActiveLoggerCmdDict.xml
    include "ActiveLoggerCmdDict.fppi"

    @ FPP from XML: original path was Svc/ActiveLogger/ActiveLoggerEvrDict.xml
    include "ActiveLoggerEvrDict.fppi"

    @ FPP from XML: original path was Svc/ActiveLogger/ActiveLoggerIntIFDict.xml
    include "ActiveLoggerIntIFDict.fppi"

    @ Telemetry input port
    sync input port LogRecv: Fw.Log

    @ Packet send port
    output port PktSend: Fw.Com

    @ FATAL event announce port
    output port FatalAnnounce: Svc.FatalEvent

    @ Ping input port
    async input port pingIn: [1] Svc.Ping

    @ Ping output port
    output port pingOut: [1] Svc.Ping

    @ Port for receiving commands
    command recv port CmdDisp

    @ Port for sending command registration requests
    command reg port CmdStatus

    @ Port for sending command responses
    command resp port cmdResponseOut

    @ Port for emitting events
    event port Log

    @ Port for emitting text events
    text event port LogText

    @ Port for getting the time
    time get port timeGetOut
    
  }

}
